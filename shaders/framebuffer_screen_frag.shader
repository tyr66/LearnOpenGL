#version 330 core

struct Material
{
    sampler2D texture_diffuse0;
};

in vec2 TexCoord;
out vec4 FragColor;

uniform Material material;

// 颜色反转
vec3 inversion();
// 灰度
vec3 grayScale();

const float offset = 1.0 / 300.0;
// 锐化
vec3 sharpen();
// 模糊
vec3 blur();
// 边缘检测
vec3 edgeDetection();

void main()
{
    FragColor = vec4(edgeDetection(), 1.0);
}


vec3 inversion()
{
    vec3 color = texture(material.texture_diffuse0, TexCoord).rgb;
    return 1.0 - color;
}

vec3 grayScale()
{
    vec3 color = texture(material.texture_diffuse0, TexCoord).rgb;
    float average = (color.r + color.g + color.b) / 3.0;
    return vec3(average, average, average);
}

vec3 sharpen()
{
    float kernel[9] = {
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    };

    vec2 offsets[9] = {
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    };

    vec3 color;
    for (int i = 0; i < 9; i++) {
        color += texture(material.texture_diffuse0, TexCoord + offsets[i]).rgb * kernel[i];
    }

    return color;
}

vec3 blur()
{
    float kernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    };

    vec2 offsets[9] = {
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    };

    vec3 color;
    for (int i = 0; i < 9; i++) {
        color += texture(material.texture_diffuse0, TexCoord + offsets[i]).rgb * kernel[i];
    }

    return color;
}

vec3 edgeDetection()
{
    float kernel[9] = {
        1, 1, 1,
        1, -8, 1,
        1, 1, 1,
    };

    vec2 offsets[9] = {
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    };

    vec3 color;
    for (int i = 0; i < 9; i++) {
        color += texture(material.texture_diffuse0, TexCoord + offsets[i]).rgb * kernel[i];
    }

    return color;
}
