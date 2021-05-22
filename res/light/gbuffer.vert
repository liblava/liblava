#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "data.inc"

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(push_constant) uniform PushConstants {
    PushConstantData pc;
};

layout(binding = 0) uniform Ubo {
    UboData ubo;
};

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;

void main() {
    outPos = vec3(pc.model * vec4(inPos, 1.0));
    outUV = inUV;

    outNormal = normalize(mat3(pc.model) * inNormal);
    // correctly render double-sided materials
    vec3 V = normalize(ubo.camPos - outPos);
    if (dot(outNormal, V) < 0.0)
        outNormal = -outNormal;

    gl_Position = ubo.projection * ubo.view * vec4(outPos, 1.0);
}
