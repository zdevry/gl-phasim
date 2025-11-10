#version 460 core

in vec2 screenSpace;

out vec4 colour;

layout(std140, binding = 0) uniform GridData {
    int width;
    int height;
};

layout(std430, binding = 0) buffer GridBuffer {
    int grid[];
};

void main() {
    ivec2 cellIJ = clamp(
            ivec2(screenSpace * vec2(width, height)),
            ivec2(0, 0), ivec2(width - 1, height - 1));

    int isFilled = grid[cellIJ.x + cellIJ.y * width];
    colour = vec4(vec3(isFilled) * vec3(0.0, 0.5, 1.0), 1.0);
}
