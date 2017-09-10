#version 330 core

in vec2 in_pos;
in vec2 in_texcoord;
in vec4 in_color;

out vec2 f_texcoord;
out vec4 f_color;

uniform vec2 u_VP;

void main(void) {
    gl_Position = vec4((in_pos * u_VP) + vec2(-1.0 -1.0), 1.0, 1.0);
    f_color = in_color;
    f_texcoord = in_texcoord;
}
