#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Pos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 viewPos;
uniform mat4 view;

void main()
{
    vec3 view_lightPos = vec3(view * vec4(lightPos, 1.0));

    float ambientFactor = 0.1;
    float shiness = 32;
    float specularStrength = 0.5;
    vec3 lightDir = normalize(view_lightPos - Pos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-Pos);

    vec3 ambient = ambientFactor * lightColor;
    vec3 diffuse = max(dot(lightDir,norm), 0.0) * lightColor;
    vec3 specular = pow(max(dot(reflect(-lightDir, norm), viewDir), 0.0), shiness) * specularStrength * lightColor;
    vec3 color = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(color, 1.0);
}
