#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 pos;
    vec3 normal;
    vec4 lightSpacePos;
    vec2 texCoord;
} fs_in;

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 lightDir;
};

struct Material
{
    sampler2D texture_diffuse0;
};

#define MAX_DIR_LIGHT 15
uniform int dirlightCnt;
uniform DirectionalLight dirLights[MAX_DIR_LIGHT];
uniform Material material;
uniform sampler2D shadow_texture;
uniform vec3 viewPos;

float caulateShadow(vec3 nor)
{
    vec3 tex = fs_in.lightSpacePos.xyz / fs_in.lightSpacePos.w;
    tex = tex * 0.5 + 0.5;

    if (tex.z > 1.0)
        return 0.0;

    float shadowDepth = texture(shadow_texture, tex.xy).r;
    // 阴影偏移
    float bias = max(0.05 * dot(dirLights[0].lightDir, nor), 0.005);
    float currentDepth = tex.z;
    float shadow = 0.0;
    //return shadowDepth + bias < currentDepth ? 1.0 : 0.0;
    // PCM 软阴影
    vec2 texSize = 1.0 / textureSize(shadow_texture, 0);

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadow_texture, tex.xy + vec2(x, y) * texSize).r;
            shadow += pcfDepth + bias < currentDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}


void main()
{
    vec3 color = texture(material.texture_diffuse0, fs_in.texCoord).rgb;
    vec3 finalColor = color;
    vec3 lightDir = dirLights[0].lightDir;
    vec3 nor = normalize(fs_in.normal);

    vec3 ambient = 0.3 * color * dirLights[0].ambient;

    vec3 diffuse = max(dot(-lightDir, nor), 0.0) * dirLights[0].diffuse * color;

    vec3 half_vec = normalize(viewPos - fs_in.pos) + dirLights[0].lightDir ;
    half_vec = normalize(half_vec);

    float specularFactor = pow(max(dot(half_vec, nor), 0.0), 64.0);
    vec3 specular = specularFactor * dirLights[0].specular *  color;

    float shadowFactory = caulateShadow(nor);

    finalColor = ambient + (1.0 - shadowFactory) * (diffuse + specular);

    FragColor = vec4(finalColor, 1.0);
}
