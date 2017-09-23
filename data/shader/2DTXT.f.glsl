#version 330 core

in vec2 f_tex;
in vec4 f_col;

uniform sampler2D u_tex;

void main(void) {
    gl_FragColor = vec4(f_col.rgb, f_col.a * texture2D(u_tex, f_tex).r);
}
