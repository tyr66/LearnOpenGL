#version 330 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;


void main()
{
    gl_Position = proj * view * model * vec4(pos.xyz, 1.0);
    TexCoord = texCoord;
}
