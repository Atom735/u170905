#version 330 core

in vec2 v_pos;
in vec2 v_tex;
in vec4 v_col;

uniform vec2 u_mat;
uniform vec2 u_pos;
uniform vec2 u_tsz;

out vec2 f_tex;
out vec4 f_col;

void main(void) {
    gl_Position = vec4(((v_pos + u_pos) * u_mat) + vec2(-1.0, 1.0), 0.0, 1.0);
    f_tex = v_tex*u_tsz;
    f_col = v_col;
}
