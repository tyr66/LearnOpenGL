#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Pos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 viewPos;

void main()
{
    float ambientFactor = 0.1;
    float shiness = 32;
    float specularStrength = 0.5;
    vec3 lightDir = normalize(lightPos - Pos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - Pos);

    vec3 ambient = ambientFactor * lightColor;
    vec3 diffuse = max(dot(lightDir,norm), 0.0) * lightColor;
    vec3 specular = pow(max(dot(reflect(-lightDir, norm), viewDir), 0.0), 32) * specularStrength * lightColor;
    vec3 color = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(color, 1.0);
}
