#version 150 core
in vec2 Texcoord;
uniform sampler2D tex_1;
out vec4 out_Color0;
// layout (location=0) out vec4 out_Color0;
// layout (location=1) out vec4 out_Color1;
// layout (location=2) out vec4 out_Color2;

void main() {
    out_Color0 = texture(tex_1, Texcoord);
    // out_Color1 = vec4(0.0);
    // out_Color2 = vec4(1.0);
}