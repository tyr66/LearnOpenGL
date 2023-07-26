#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 lightPos;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;

    float shiness;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 Pos;
in vec2 TexCoord;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;
uniform mat4 view;

void main()
{
    vec3 view_lightPos = vec3(view * vec4(light.lightPos, 1.0));

    vec3 lightDir = normalize(view_lightPos - Pos);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-Pos);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord);

    vec3 diffuse = max(dot(lightDir,norm), 0.0) * light.diffuse * texture(material.diffuse, TexCoord);
    vec3 specular = pow(max(dot(reflect(-lightDir, norm), viewDir), 0.0), material.shiness) * light.specular * texture(material.specular, TexCoord);

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
