#version 460 core

in vec2 texCoord;

out vec4 colour;

uniform sampler2D textsheet;

void main() {
    float alpha = texture(textsheet, texCoord).r;
    colour = mix(vec4(0.0, 0.0, 0.0, 0.5), vec4(1.0), alpha);
}
