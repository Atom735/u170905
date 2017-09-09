#version 330 core

in vec2 position;
in vec4 color;

out vec4 fragmentColor;

void main(void) {
    gl_Position   = vec4(position, 1.0, 1.0);
    fragmentColor = color;
}