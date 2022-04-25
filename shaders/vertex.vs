#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 tCoord;

out vec3 texCoord;

void main() {
    gl_Position = vec4(pos, 1.0);
    texCoord = tCoord;
}