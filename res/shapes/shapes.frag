#version 450 core

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outFragColor;

void main() {
    outFragColor = vec4((inNormal.x + 1) / 2,
                        (inNormal.y + 1) / 2,
                        (inNormal.z + 1) / 2, 1);
}
