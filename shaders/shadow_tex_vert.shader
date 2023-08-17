#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEXCOORD;

uniform mat4 model;

void main()
{
    gl_Position =  model * vec4(POS, 1.0);
}
