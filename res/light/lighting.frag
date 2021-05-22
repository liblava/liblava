#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "data.inc"

layout(binding = 0) uniform sampler2D samplerGbufferAlbedo;
layout(binding = 1) uniform sampler2D samplerGbufferNormal;
layout(binding = 2) uniform sampler2D samplerGbufferMetallicRoughness;
layout(binding = 3) uniform sampler2D samplerGbufferDepth;

layout(binding = 4) uniform Ubo {
    UboData ubo;
};

layout(binding = 5) restrict readonly buffer Sbo_Lights {
    LightData lights[];
};

layout(location = 0) out vec4 outFragColor;

// Physically based shading, metallic + roughness workflow (GLTF 2.0 core material spec)
// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#metallic-roughness-material
// Some GLSL code adapted from
// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation
// https://google.github.io/filament/Filament.md.html

#define INV_PI 0.31831

struct Material {
    vec3 diffuse;
    vec3 f0;
    float a;
};

// Schlick approximation to Fresnel equation using F90 = 1
// Schlick 1994. An Inexpensive BRDF Model for Physically based Rendering.
vec3 F_Schlick(float VoH, vec3 F0) {
    float f = pow(1.0 - VoH, 5.0);
    return f + F0 * (1.0 - f);
}

// GGX Normal Distribution Function
// Bruce Walter et al. 2007. Microfacet Models for Refraction through Rough Surfaces.
float D_GGX(float NoH, float a) {
    a = NoH * a;
    float k = a / (1.0 - NoH * NoH + a * a);
    return k * k * INV_PI;
}

// GGX Geometric Shadowing/Occlusion Function, based on Smith approach
// height-correlated joint masking-shadowing function
// Heitz 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs.
float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

// Lambertian diffuse BRDF
vec3 diffuseBrdf(Material mat, float VoH) {
    vec3 F = F_Schlick(VoH, mat.f0);
    return (1.0 - F) * mat.diffuse * INV_PI;
}

vec3 specularBrdf(Material mat, float NoH, float VoH, float NoV, float NoL) {
    float D = D_GGX(NoH, mat.a);
    vec3 F = F_Schlick(VoH, mat.f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, mat.a);

    return (D * V) * F;
}

// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation
vec3 brdf(Material mat, vec3 v, vec3 l, vec3 n) {
    vec3 h = normalize(l + v);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float VoH = clamp(dot(v, h), 0.0, 1.0);

    return specularBrdf(mat, NoH, VoH, NoV, NoL) + diffuseBrdf(mat, VoH);
}

// inverse square falloff
float distanceAttenuation(float distance) {
    return 1.0 / max(distance * distance, 0.01 * 0.01);
}

// windowing function with smooth transition to 0
// radius is arbitrary
// Karis 2014. Real Shading in Unreal Engine 4.
float distanceAttenuation(float distance, float radius) {
    float nom = clamp(1.0 - pow(distance / radius, 4.0), 0.0, 1.0);
    return nom * nom * distanceAttenuation(distance);
}

vec3 screenToEye(vec3 fragCoord, uvec2 resolution, mat4 invProjection) {
    vec3 ndc = vec3(
        2.0 * fragCoord.xy / vec2(resolution) - 1.0, // -> [-1;1]
        fragCoord.z // [0;1]
    );
    vec4 eye = invProjection * vec4(ndc, 1.0);
    return eye.xyz / eye.w;
}

// luminance-only fit of ACES LDR output transform
// Narkowicz 2016. ACES Filmic Tone Mapping Curve.
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 tonemap(vec3 x) {
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(ubo.resolution);
    vec3 albedo = texture(samplerGbufferAlbedo, uv).rgb;
    vec2 metallicRoughness = texture(samplerGbufferMetallicRoughness, uv).xy;
    float metallic = metallicRoughness.x;
    float roughness = metallicRoughness.y;

    // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#metallic-roughness-pbr_mat

    const vec3 dielectricSpecular = vec3(0.04, 0.04, 0.04);
    const vec3 black = vec3(0.0, 0.0, 0.0);

    Material mat;
    mat.diffuse = mix(albedo * (vec3(1.0) - dielectricSpecular), black, metallic);
    mat.f0 = mix(dielectricSpecular, albedo, metallic);
    mat.a = roughness * roughness;

    // restore fragment coordinate from depth
    float z = texture(samplerGbufferDepth, uv).x;
    vec3 fragCoord = vec3(gl_FragCoord.xy, z);
    vec3 pos = screenToEye(fragCoord, ubo.resolution, ubo.invProjection);

    vec3 V = normalize(ubo.camPos - pos);
    vec3 N = texture(samplerGbufferNormal, uv).xyz;

    vec3 radiance = vec3(0.0);
    for (uint i = 0; i < ubo.lightCount; ++i) {
        LightData light = lights[i];
        float dist = distance(light.position, pos);
        float attenuation = distanceAttenuation(dist, light.radius);
        if (attenuation > 0.0) {
            vec3 L = normalize(light.position - pos);
            vec3 radiance_in = light.intensity * attenuation;

            float NoL = clamp(dot(N, L), 0.0, 1.0);
            if (NoL > 0.0)
                radiance += brdf(mat, V, L, N) * radiance_in * NoL;
        }
    }

    outFragColor = vec4(tonemap(radiance), 1.0);
}
