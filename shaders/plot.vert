#version 460 core

layout(std140, binding = 2) uniform PlotData {
    vec2 origin; // 0
    vec2 scale; // 8
    float lineWidth; // 16
    int startIndex; // 20
    int bufferLength;
};

layout(std430, binding = 2) buffer PlotBuffer {
    float plotVals[];
};

void main() {
    int index = gl_VertexID / 2;
    int bufferIndex = (startIndex + index) % bufferLength;

    vec2 pos = vec2(index, plotVals[bufferIndex]) * scale + origin;
    pos.y += (float(gl_VertexID & 1) - 0.5) * lineWidth;
    gl_Position = vec4(pos, 0.0, 1.0);
}
