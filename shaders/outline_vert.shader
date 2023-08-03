#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float outlineScale;

out vec2 texCoord;
out vec3 normal;
out vec3 pos;

void main()
{
    vec3 scale = normalize(Normal) * outlineScale; // 顶点往法线方向的偏移

    gl_Position = proj * view * model * vec4(Pos + scale  , 1.0);
    texCoord = TexCoord;
    pos = vec3(view * model * vec4(Pos + scale , 1.0));
    normal = vec3(view * model * vec4(Normal, 0.0));
    normal = normalize(normal);
}
