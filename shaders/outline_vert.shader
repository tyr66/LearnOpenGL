#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 texCoord;
out vec3 normal;
out vec3 pos;

void main()
{
    gl_Position = proj * view * model * vec4(Pos , 1.0);
    texCoord = TexCoord;
    pos = vec3(view * model * vec4(Pos , 1.0));
    normal = vec3(view * model * vec4(Normal, 0.0));
    normal = normalize(normal);
}
