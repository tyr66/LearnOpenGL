#version 330 core

in vec4 FragPos;
uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    vec3 toDir = lightPos - FragPos.xyz;
    float dist = length(toDir);

    gl_FragDepth = dist / far_plane;
}
