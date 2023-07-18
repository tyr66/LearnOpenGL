#version 330 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 tex;

out vec4 ourColor;
out vec2 texCoord;
uniform mat4 transform;

void main()
{
    gl_Position = transform * pos;
    ourColor = color;
    texCoord = tex;
}
