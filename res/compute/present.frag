#version 450
#extension GL_ARB_separate_shader_objects : enable

// layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform sampler2D image;

void main() {
    FragColor = texture(image, TexCoord);
}
