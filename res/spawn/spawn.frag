#version 450 core

layout(binding = 2) uniform sampler2D samplerColor;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outFragColor;

void main() {
    outFragColor = inColor * texture(samplerColor, inUV);
}
