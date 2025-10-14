#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 TexCoord;

void main() {
    TexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(TexCoord * 2.0f - 1.0f, 0.0f, 1.0f); 
}
