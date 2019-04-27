#version 150 core
out vec4 out_Color0;
// layout (location=0) out vec4 out_Color0;
// layout (location=1) out vec4 out_Color1;
// layout (location=2) out vec4 out_Color2;

void main() {
    out_Color0 = vec4(0.5, 0.25, 0.75, 1.0);
    // out_Color1 = vec4(0.0);
    // out_Color2 = vec4(1.0);
}