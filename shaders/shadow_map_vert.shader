#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEXCOORD;

out VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
} vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(POS, 1.0);
    vs_out.pos = vec3(model * vec4(POS, 1.0));
    vs_out.normal = normalize(vec3(transpose(inverse(model)) * vec4(NORMAL, 0.0)));
    vs_out.texCoord = TEXCOORD;
}
