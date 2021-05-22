#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "data.inc"

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(push_constant) uniform PushConstants {
    PushConstantData pc;
};

layout(binding = 1) uniform sampler2D samplerNormal;
layout(binding = 2) uniform sampler2D samplerRoughness;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec2 outMetallicRoughness;

// Schlüter 2013. Normal Mapping Without Precomputed Tangents.
// http://www.thetenthplanet.de/archives/1180
mat3 cotangentFrame(vec3 N, vec3 pos, vec2 uv) {
    vec3 dPx = dFdx(pos);
    vec3 dPy = dFdy(pos);
    vec2 dTx = dFdx(uv);
    vec2 dTy = dFdy(uv);

    vec3 dPxC = cross(N, dPx);
    vec3 dPyC = cross(dPy, N);

    vec3 T = dPyC * dTx.x + dPxC * dTy.x;
    vec3 B = dPyC * dTx.y + dPxC * dTy.y;

    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));

    return mat3(T * invmax, B * invmax, N);
}

void main() {
    vec3 normal = normalize(inNormal);
    if (pc.enableNormalMapping != 0) {
        mat3 TBN = cotangentFrame(normal, inPos, inUV);
        vec3 tangentNormal = texture(samplerNormal, inUV).xyz * 2.0 - 1.0;
        normal = normalize(TBN * tangentNormal);
    }

    float roughness = texture(samplerRoughness, inUV).x;

    outAlbedo = vec4(pc.color, 1.0);
    outNormal = vec4(normal, 1.0);
    outMetallicRoughness = vec2(pc.metallic, roughness);
}
