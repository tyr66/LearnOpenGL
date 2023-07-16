#version 330 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
out vec4 ourColor;

void main()
{
    gl_Position = pos;
    ourColor = color;
}
