#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

#include "glm/glm.hpp"

struct Light;
struct Material;

class Shader {

private:
    Shader(unsigned int id);
public:
    static std::unique_ptr<Shader> CreateShader(const char* vertShaderPath, const char* fragShaderPath);
    void Bind();
    void Unbind();
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
    ~Shader();
private:
    unsigned int _renderID{0};
    std::unordered_map<std::string, int> _uniformLocation;
};
