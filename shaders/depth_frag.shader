#version 330 core

struct Material
{
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;

    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    
    float shiness;
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
float near = 0.1;
float far = 5.0;

float LinearDepth(float depth);

void main()
{             
    float depth = (LinearDepth(gl_FragCoord.z) - near)/ (far - near); 
    FragColor = vec4(depth ,depth, depth, 1.0);
}
float LinearDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
