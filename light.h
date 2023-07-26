#pragma once
#include <glm/glm.hpp>
#include <string>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float shiness;

    std::string name;
};

struct DirectionalLight
{
    glm::vec3 lightDir;
    glm::vec3 lightDirView;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    std::string name;
};

struct PointLight {

    glm::vec3 lightPos;
    glm::vec3 lightPosView;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    std::string name;
};

struct SpotLight{

    glm::vec3 lightPos;
    glm::vec3 lightPosView;
    glm::vec3 lightDir;
    glm::vec3 lightDirView;

    float constant;
    float linear;
    float quadratic;
    float outerCutoff;
    float cutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    std::string name;
};
