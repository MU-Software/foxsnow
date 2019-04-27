#version 150 core
in vec2 Texcoord;
uniform sampler2D screen_tex;
out vec4 out_Color0;

void main() {
    out_Color0 = texture(screen_tex, Texcoord);
}