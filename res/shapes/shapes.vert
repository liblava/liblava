#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(binding = 0) uniform Ubo_Camera {
    mat4 projection;
    mat4 view;
}
ubo_camera;

layout(binding = 1) uniform Ubo_Spawn {
    mat4 model;
}
ubo_spawn;

layout(binding = 2) uniform Ubo_Rotation {
    vec3 rotation;
}
ubo_rotation;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

mat3 make_rotation_matrix(float theta) {
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);
    return mat3(
        cos_theta, 0.0, -sin_theta,
        0.0, 1.0, 0.0,
        sin_theta, 0.0, cos_theta);
}

void main() {
    outColor = inColor;
    outNormal = inNormal;

    gl_Position = ubo_camera.projection * ubo_camera.view * ubo_spawn.model
                  * vec4(make_rotation_matrix(ubo_rotation.rotation.y) * inPos, 1.0);
}
