#pragma once
#include <glm/glm.hpp>
#include <string>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float shiness;
};

struct DirectionalLight
{
    glm::vec3 lightDir;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};

struct PointLight {

    glm::vec3 lightPos;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};

struct SpotLight{

    glm::vec3 lightPos;
    glm::vec3 lightDir;

    float constant;
    float linear;
    float quadratic;
    float outerCutoff;
    float cutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};
