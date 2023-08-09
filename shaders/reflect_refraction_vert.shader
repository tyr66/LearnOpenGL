#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEXCOORD;

out vec3 Pos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    Pos = (model * vec4(POS, 1.0)).xyz;
    Normal = (model * vec4(NORMAL, 0.0)).xyz;

    gl_Position = proj * view * model * vec4(POS, 1.0);
}
