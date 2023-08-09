#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec4 normal;
} gs_in[];

uniform mat4 proj;
const float NORMAL_LENGTH = 0.4;

void generateNormal(int i)
{
    gl_Position = proj * gl_in[i].gl_Position;
    EmitVertex();

    gl_Position = proj * (gl_in[i].gl_Position + vec4(gs_in[i].normal * NORMAL_LENGTH));
    EmitVertex();

    EndPrimitive();
}

void main()
{
    generateNormal(0);
    generateNormal(1);
    generateNormal(2);
}
