#version 330 core

in vec2 f_texcoord;
in vec4 f_color;

uniform sampler2D u_tex0;

out vec4 out_color;

void main(void) {
    out_color = f_color * texture2D(u_tex0, f_texcoord);;
}
