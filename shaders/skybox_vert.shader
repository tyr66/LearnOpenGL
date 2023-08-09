#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 view;
uniform mat4 proj;

out vec3 TexCoord;

void main(){
    TexCoord = pos;
    vec4 fakePos = proj * view * vec4(pos, 1.0);
    gl_Position = fakePos.xyww;
}
