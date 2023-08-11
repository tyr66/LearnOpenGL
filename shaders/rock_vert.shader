#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEXCOORD;
layout (location = 3) in mat4 model;

uniform mat4 proj;
uniform mat4 view;

out VS_OUT{
    vec2 TexCoord;
} vs_out;

void main()
{
    gl_Position = proj * view * model * vec4(POS, 1.0);
    vs_out.TexCoord = TEXCOORD;
}
