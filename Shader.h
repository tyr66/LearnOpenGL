#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

#include "glm/glm.hpp"
#include "Mesh.h"

struct DirectionalLight;
struct SpotLight;
struct PointLight;

struct Material;

class Shader {

private:
    Shader(unsigned int id);
public:
    static std::unique_ptr<Shader> CreateShader(const char* vertShaderPath, const char* fragShaderPath);
    void Bind();
    void Unbind();

    void ResetLightIdx();
    void SetInt(std::string&name, int value);
    void SetFloat(std::string& name, float value);
    void SetInt(std::string&&name, int value);
    void SetFloat(std::string&& name, float value);
    void SetMat4f(std::string& name, const float* data);
    void SetMat4f(std::string&& name, const float* data);
    void SetVec3f(std::string& name, const glm::vec3& v);
    void SetVec3f(std::string&& name, const glm::vec3& v);
    /* 
    void SetLight(std::string& name, const Light& light);
    void SetLight(std::string&& name, const Light& light);
    void SetMaterial(std::string& name, const Material& material);
    void SetMaterial(std::string&& name, const Material& material); */
    void SetDirectionalLight(const DirectionalLight& light);
    void SetSpotLight(const SpotLight& light);
    void SetPointLight(const PointLight& light);
    void SetDirectionalLightNum(int num);
    void SetPointLightNum(int num);
    void SetSpotLightNum(int num);
    ~Shader();

public:
    static const char* DiffuseTexturePrefix;
    static const char* SpecularTexturePrefix;

    static const int Max_DirectionalLight_Cnt = 20;
    static const int Max_PointLight_Cnt = 20;
    static const int Max_SpotLight_Cnt = 20;

private:
    static std::string GetDirectionLightPrefix(int idx);
    static std::string GetPointLightPrefix(int idx);
    static std::string GetSpotLightPrefix(int idx);

    int getAndSetLocation(const std::string& name);
    int getAndSetLocation(const std::string&& name);

private:
    unsigned int _renderID{0};
    std::unordered_map<std::string, int> _uniformLocation;
    int _directionalLightIdx{0};
    int _pointLightIdx{0};
    int _spotLightIdx{0};
};
