#version 330 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tex;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
    gl_Position = proj * view * model * vec4(pos.xyz, 1.0f);
    texCoord = tex;
}
