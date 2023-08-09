#version 330 core

in vec3 Pos;
in vec3 Normal;
out vec4 FragColor;


uniform vec3 viewPos;
uniform samplerCube skybox;

vec3 reflection();
vec3 refraction();

void main() {
    FragColor = vec4(refraction(), 1.0);
}

vec3 reflection()
{
    vec3 I = normalize(Pos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    return texture(skybox, R);
}

vec3 refraction()
{
    float refractionIndex = 1.00 / 1.52;
    vec3 I = normalize(Pos - viewPos);
    vec3 R = refract(I, normalize(Normal), refractionIndex);
    return texture(skybox, R);
}
