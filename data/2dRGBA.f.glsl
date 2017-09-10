#version 330 core

in vec4 fragmentColor;

out vec4 out_color;

void main(void) {
    out_color = fragmentColor;
}
