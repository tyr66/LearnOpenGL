#version 330 core

in vec3 TexCoord;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    //vec3 dir = vec3(TexCoord.x, -TexCoord.y, TexCoord.z);
    FragColor = texture(skybox, TexCoord);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
