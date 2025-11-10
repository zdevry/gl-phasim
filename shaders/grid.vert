#version 460 core

out vec2 screenSpace;

void main() {
    int i = gl_VertexID & 1;
    int jx2 = gl_VertexID & 2;

    gl_Position = vec4(i * 2.0 - 1.0, 1.0 - jx2, 0.0, 1.0);
    screenSpace = vec2(i, jx2 / 2.0);
}
