#version 330 core

in vec2 in_pos;
in vec4 in_color;

out vec4 fragmentColor;

void main(void) {
    gl_Position   = vec4(in_pos, 1.0, 1.0);
    fragmentColor = in_color;
}
