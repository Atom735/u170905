#version 330 core

in vec2 v_pos;
in vec2 v_tex;

uniform vec2 u_mat;

out vec2 f_tex;

void main(void) {
    gl_Position = vec4((v_pos * u_mat) + vec2(-1.0, -1.0), 0.0, 1.0);
    f_tex = v_tex;
}
