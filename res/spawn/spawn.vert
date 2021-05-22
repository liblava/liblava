#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;

layout(binding = 0) uniform Ubo_Camera {
    mat4 projection;
    mat4 view;
}
ubo_camera;

layout(binding = 1) uniform Ubo_Spawn {
    mat4 model;
}
ubo_spawn;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    outColor = inColor;
    outUV = inUV;

    gl_Position = ubo_camera.projection * ubo_camera.view * ubo_spawn.model * vec4(inPos, 1.0);
}
