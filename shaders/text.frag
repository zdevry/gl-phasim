#version 460 core

in vec2 texCoord;

out vec4 colour;

uniform sampler2D textsheet;

void main() {
    float alpha = texture(textsheet, texCoord).r;
    colour = vec4(1.0, 1.0, 1.0, alpha);
}
