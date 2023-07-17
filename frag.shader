#version 330 core

in vec4 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec2 texCoord2 = vec2(1.0 - texCoord.x, 1.0 - texCoord.y);
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord2), 0.2);
    //FragColor = texture(texture1, texCoord);
}

