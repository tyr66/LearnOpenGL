#version 330 core

out vec4 FragColor;

struct Material
{
    sampler2D texture_diffuse0;
    sampler2D texture_normal0;
    sampler2D texture_parallax0;
};

struct PointLight {
    vec3 lightPos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
#define MAX_POINT_LIGHT 5
uniform int pointLightCnt;
uniform PointLight pointLights[MAX_POINT_LIGHT];

uniform Material material;
uniform float far_plane;
uniform vec3 viewPos;
uniform samplerCube shadow_texture;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

in VS_OUT
{
    vec3 pos;
    vec2 tex;
    vec3 normal;
    vec3 tangent;
} fs_in;

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

vec3 normalMapping(mat3 TBN, vec2 tex)
{
    vec3 nor = texture(material.texture_normal0, tex).rgb;
    nor = nor * 2.0 - 1.0;
    nor = TBN * nor;
    return normalize(nor);
}

vec2 parallaxMapping(vec3 dir)
{
    dir = normalize(dir);
    float height_scale = 0.1;
    float numsLayer = mix(32.0, 8.0, max(dot(vec3(0, 0, 1), dir), 0));
    float deltaDepth = 1.0 / numsLayer;
    vec2 P = dir.xy * height_scale;
    vec2 deltaTex = P / numsLayer;

    vec2 currentTex = fs_in.tex;
    float currentDepth = 0.0;
    float sampleDepth = texture(material.texture_parallax0, currentTex).r;

    while (currentDepth < sampleDepth) {
        currentDepth += deltaDepth;
        currentTex -= deltaTex;
        sampleDepth = texture(material.texture_parallax0, currentTex).r;
    }

    //return currentTex;

    // 进行线性插值, 避免出现分层
    
    vec2 prevTex = currentTex + deltaTex;
    float afterDepth = currentDepth - sampleDepth;
    float beforeDepth = texture(material.texture_parallax0, prevTex).r - currentDepth + deltaDepth;
    float weight = afterDepth / (afterDepth + beforeDepth);

    vec2 tex = currentTex + (prevTex - currentTex) * weight;

    return tex;
}

void main()
{
    vec3 T = normalize(fs_in.tangent);
    vec3 N = normalize(fs_in.normal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    mat3 invTBN = transpose(TBN);

    vec3 toViewDir = normalize(viewPos - fs_in.pos);

    vec2 tex = parallaxMapping(invTBN * toViewDir);
    vec3 nor = normalMapping(TBN, tex);
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    vec3 lightDir = normalize(fs_in.pos - pointLights[0].lightPos);
    vec3 color = texture(material.texture_diffuse0, tex).rgb;

    vec3 ambient = 0.3 * pointLights[0].ambient;

    vec3 diffuse = max(dot(-lightDir, nor), 0.0) * pointLights[0].diffuse;

    vec3 half_vec = normalize(-lightDir + toViewDir);
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
