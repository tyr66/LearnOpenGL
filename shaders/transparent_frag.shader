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

struct DirectionalLight
{
    vec3 lightDir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 lightPos;
    vec3 lightDir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

    float outerCutOff;
    float cutOff;
};

struct PointLight
{
    vec3 lightPos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

out vec4 FragColor;

in vec3 pos; // 摄像机坐标系下的坐标
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture_diffuse0;
uniform Material material;

#define MAX_DIR_LIGHT 20
#define MAX_POINT_LIGHT 20
#define MAX_SPOT_LIGHT 22

uniform DirectionalLight dirLights[MAX_DIR_LIGHT];
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform SpotLight spotLights[MAX_SPOT_LIGHT];
uniform int dirLightCnt;
uniform int pointLightCnt;
uniform int spotLightCnt;


vec3 calDirectionalLight(DirectionalLight light, vec3 nor, vec3 viewDir);
vec3 calPointLight(PointLight light, vec3 nor, vec3 viewDir, vec3 fragPos);
vec3 calSpotLight(SpotLight light, vec3 nor, vec3 viewDir, vec3 fragPos);


vec3 diffuseSample;
vec3 ambientSample;
vec3 specularSample;

vec3 defaultSpecular = vec3( 0.0, 0.0, 0.0);
vec3 defaultDiffuse = vec3(1.0, 1.0, 1.0);

void main()
{
    float alpha = texture(material.texture_diffuse0, texCoord).a;
    ambientSample = texture(material.texture_diffuse0, texCoord).rgb * material.enableDiffuse;
    diffuseSample = texture(material.texture_diffuse0, texCoord).rgb * material.enableDiffuse;
    specularSample = texture(material.texture_specular0, texCoord).rgb * material.enableSpecular;

    ambientSample += (1 - material.enableDiffuse) * defaultDiffuse;
    diffuseSample += (1 - material.enableDiffuse ) * defaultDiffuse;
    specularSample += (1 - material.enableSpecular ) * defaultSpecular;

    vec3 viewDir = normalize(-pos);

    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < dirLightCnt; i++)
        color += calDirectionalLight(dirLights[i], normal, viewDir);

    for (int i = 0; i < pointLightCnt; i++)
        color += calPointLight(pointLights[i], normal, viewDir, pos);

    for (int i = 0; i < spotLightCnt; i++)
        color += calSpotLight(spotLights[i], normal, viewDir, pos);

    FragColor = vec4(color, alpha);
    //FragColor = vec4(dirLights[0].lightDir, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

vec3 calDirectionalLight(DirectionalLight light, vec3 nor, vec3 viewDir)
{
    float diffuseFactory = max(dot(nor, -light.lightDir), 0.0);

    vec3 reflectDir = reflect(light.lightDir, nor);
    float specularFactory = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = light.diffuse * diffuseSample;
    vec3 specular = light.specular * specularSample;

    diffuse *= diffuseFactory;
    specular *= specularFactory;

    return ambient + diffuse + specular;
}

vec3 calPointLight(PointLight light, vec3 nor, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(fragPos - light.lightPos);
    float diffuseFactory = max(dot(nor, -lightDir), 0.0);

    vec3 reflectDir = reflect(lightDir, nor);
    float specularFactory = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = diffuseFactory * light.diffuse * diffuseSample;
    vec3 specular = specularFactory * light.specular * specularSample;

    float len = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * len + light.quadraticAttenuation * len * len);

    return attenuation * (ambient + diffuse + specular);
}

vec3 calSpotLight(SpotLight light, vec3 nor, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(fragPos - light.lightPos);
    float diffuseFactory = max(dot(nor, -lightDir), 0.0);

    vec3 reflectDir = reflect(lightDir, nor);
    float specularFactory = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = light.ambient * ambientSample;
    vec3 diffuse = diffuseFactory * light.diffuse * diffuseSample;
    vec3 specular = specularFactory * light.specular * specularSample;

    float len = length(light.lightPos - fragPos);
    float pointAttenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * len + light.quadraticAttenuation * len * len);

    float theta = dot(lightDir, light.lightDir);
    float spotAttenuation = (theta - light.outerCutOff) / (light.cutOff - light.outerCutOff);
    spotAttenuation = clamp(spotAttenuation, 0.0, 1.0);

    return spotAttenuation * pointAttenuation * (ambient + diffuse + specular);
}

