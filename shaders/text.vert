#version 460 core

out vec2 texCoord;

layout(std140, binding = 1) uniform TextData {
    vec2 topLeft;
    float width;
    float textsheetWidth;
    float aspect;
};

layout(std430, binding = 1) buffer TextBuffer {
    int text[];
};

void main() {
    int i = gl_VertexID & 1;
    int jx2 = gl_VertexID & 2;

    vec2 vertexOffset = vec2((i + gl_InstanceID) * width / aspect, -jx2 * width);
    gl_Position = vec4(topLeft + vertexOffset, 0.0, 1.0);
    texCoord = vec2((i + text[gl_InstanceID]) / textsheetWidth, jx2 / 2.0);
}
