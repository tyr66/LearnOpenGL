#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
} fs_in;

struct PointLight {
    vec3 lightPos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct Material
{
    sampler2D texture_diffuse0;
};

#define MAX_POINT_LIGHT 15
uniform int pointLightCnt;
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform Material material;
uniform vec3 viewPos;
uniform samplerCube shadow_texture;
uniform float far_plane;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float caulateShadow(vec3 dir, float toViewDist , float dist)
{
    float currentDepth = dist;
    float shadow = 0.0;

    //float depth = texture(shadow_texture, dir ).r;
    //depth *= far_plane;
    //return currentDepth - 0.15 > depth ? 1.0 : 0.0;

    //return texture(shadow_texture, dir).r;
    int sample = 20;
    float diskRadius = (1.0 + (toViewDist / far_plane)) / 25.0;

    for (int i = 0; i < sample; i++) {
        float depth = texture(shadow_texture, dir + gridSamplingDisk[i] * diskRadius).r;
        depth *= far_plane;
        shadow += currentDepth - 0.15 > depth ? 1.0 : 0.0;
    }

    return shadow / float(sample);
}


void main()
{
    vec3 color = texture(material.texture_diffuse0, fs_in.texCoord).rgb;
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    vec3 nor = normalize(fs_in.normal);
    vec3 lightDir = normalize(fs_in.pos - pointLights[0].lightPos);

    vec3 ambient = 0.3 * pointLights[0].ambient;

    vec3 diffuse = max(dot(-lightDir, nor), 0.0) * pointLights[0].diffuse;

    vec3 half_vec = normalize(-lightDir + normalize(viewPos - fs_in.pos));
    float specularFactory = pow(max(dot(nor, half_vec), 0.0), 32.0);
    vec3 specular = specularFactory * pointLights[0].specular;

    float dist = length(fs_in.pos - pointLights[0].lightPos);
    float attenuation = 1.0 / (pointLights[0].constantAttenuation + dist * pointLights[0].linearAttenuation + dist * dist * pointLights[0].quadraticAttenuation);

    //float attenuation = 1.0 / (1.0 + 0.7 * dist + dist * dist * 1.8);
    float toViewDist = length(fs_in.pos - viewPos);
    float shadow = 1.0 - caulateShadow(fs_in.pos - pointLights[0].lightPos, toViewDist, dist);
    finalColor = (ambient + shadow * (diffuse + specular)) * color * attenuation;
    
    FragColor = vec4(finalColor, 1.0);
}
