#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEXCOORD;

out VS_OUT{
    vec4 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    vs_out.normal = normalize(transpose(inverse(view * model)) * vec4(NORMAL, 0.0));
    gl_Position = view * model * vec4(POS, 1.0);
}
