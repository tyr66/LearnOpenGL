#pragma once
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float shiness;
};

struct Light
{
    glm::vec3 lightPos;
    glm::vec3 lightColor;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
