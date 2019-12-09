#version 150 core

uniform mat4 fs_ModelMatrix;
uniform mat4 fs_ViewMatrix;
uniform mat4 fs_ProjectionMatrix;

in vec4 fs_Vertex;

void main() {
    gl_Position = fs_ProjectionMatrix
                    * fs_ViewMatrix
                    * fs_ModelMatrix
                    * fs_Vertex;
}