#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

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
    ~Shader();
private:
    unsigned int _renderID{0};
    std::unordered_map<std::string, int> _uniformLocation;
};
