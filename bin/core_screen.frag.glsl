#version 150 core
in vec2 Texcoord;
out vec4 out_Color0;
uniform sampler2D tex_1;

void main() {
    out_Color0 = texture(tex_1, Texcoord);
}