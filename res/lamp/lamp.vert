#version 450 core

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec2 outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(outUV * 2.0 + -1.0, 0.0, 1.0);
}
