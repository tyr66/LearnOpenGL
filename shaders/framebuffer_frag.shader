#version 330 core

struct Material
{
    //sampler2D texture_ambient;

    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;

    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;

    float shiness;
    int enableDiffuse;
    int enableSpecular;
};

out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse0, TexCoord);
}
