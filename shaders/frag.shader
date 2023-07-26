#version 330 core

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 lightDir;
};

struct PointLight {
    vec3 lightPos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    vec3 lightPos;
    vec3 lightDir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutoff;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;

    float shiness;
};

#define MAX_POINT_LIGHT 15

out vec4 FragColor;

in vec3 Normal;
in vec3 Pos;
in vec2 TexCoord;

uniform DirectionalLight dirLight;

uniform int pointLightCnt;
uniform PointLight pointLights[MAX_POINT_LIGHT];

uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;
uniform mat4 view;

vec3 ambientSample;
vec3 diffuseSample;
vec3 specularSample;

vec3 CalDirLight(DirectionalLight light, vec3 nor, vec3 viewDir);
vec3 CalPointLight(PointLight light, vec3 nor, vec3 viewDir, vec3 fragPos);
vec3 CalSpotLight(SpotLight light, vec3 nor, vec3 viewDir, vec3 fragPos);

void main()
{
    vec3 viewDir = normalize(viewPos - Pos);

    ambientSample = texture(material.diffuse, TexCoord).rgb;
    diffuseSample = texture(material.diffuse, TexCoord).rgb;
    specularSample = texture(material.specular, TexCoord).rgb;

    vec3 color = vec3(0.0, 0.0, 0.0);

    color += CalDirLight(dirLight, Normal, viewDir);

    for (int i = 0; i < pointLightCnt; i++) {
        color += CalPointLight(pointLights[i], Normal, viewDir, Pos);
    }

    color += CalSpotLight(spotLight, Normal, viewDir, Pos);

    FragColor = vec4(color, 1.0);
}


vec3 CalDirLight(DirectionalLight light, vec3 nor, vec3 viewDir)
{
    float diffuseFactor = max(dot(nor, -light.lightDir), 0.0);
    vec3 reflectDir = reflect(light.lightDir, nor);
    float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shiness);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = diffuseFactor * light.diffuse * diffuseSample;
    vec3 specular = specularFactor * light.specular * specularSample;

    return ambient + diffuse + specular;
}


vec3 CalPointLight(PointLight light, vec3 nor, vec3 viewDir, vec3 fragPos)
{
    float d = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);
    vec3 lightDir = normalize(light.lightPos - fragPos);

    float diffuseFactor = max(dot(nor, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, nor);
    float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shiness);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = diffuseFactor * light.diffuse * diffuseSample;
    vec3 specular = specularFactor * light.specular * specularSample;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalSpotLight(SpotLight light, vec3 nor, vec3 viewDir, vec3 fragPos)
{
    float d = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);
    vec3 lightDir = normalize(light.lightPos - fragPos);
    float theta = dot(light.lightDir, lightDir);
    float intensity = clamp((theta - light.outerCutoff) / (light.cutOff - light.outerCutoff), 0.0, 1.0);

    float diffuseFactor = max(dot(nor, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, nor);
    float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shiness);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = diffuseFactor * light.diffuse * diffuseSample;
    vec3 specular = specularFactor * light.specular * specularSample;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
