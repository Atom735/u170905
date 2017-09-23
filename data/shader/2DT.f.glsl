#version 330 core

in vec2 f_tex;

uniform sampler2D u_tex;

void main(void) {
    gl_FragColor = texture2D(u_tex, f_tex);
}
