#version 150 core

in vec2 fs_Vertex;
in vec2 fs_MultiTexCoord0;
out vec2 Texcoord;

void main() {
    Texcoord = fs_MultiTexCoord0;
    gl_Position = vec4(fs_Vertex, 0.0, 1.0);
}