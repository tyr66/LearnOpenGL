#version 330 core

layout (location = 0) in vec3 POS;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEX;
layout (location = 3) in vec3 TANGENT;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT {

    vec3 pos;
    vec2 tex;
    vec3 normal;
    vec3 tangent;

}vs_out;


void main()
{

    vs_out.pos = vec3(model * vec4(POS, 1.0));
    vs_out.tex = TEX;
    vs_out.normal = vec3(transpose(inverse(model)) * vec4(NORMAL, 0.0));
    vs_out.tangent = vec3(model * vec4(TANGENT, 0.0));

    gl_Position = proj * view * model * vec4(POS, 1.0);
}
