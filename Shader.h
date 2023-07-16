#pragma once
#include <vector>
#include <iostream>
#include <memory>

class Shader {

private:
    Shader(unsigned int id);
public:
    static std::unique_ptr<Shader> CreateShader(const char* vertShaderPath, const char* fragShaderPath);
    void Bind();
    void Unbind();
    ~Shader();
private:
    unsigned int _renderID{0};

};