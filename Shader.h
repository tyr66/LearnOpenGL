#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>

#include "glm/glm.hpp"
#include "Mesh.h"

struct DirectionalLight;
struct SpotLight;
struct PointLight;

struct Material;
class ShaderPtr;
class Shader;

class ShaderPtr {
public:
    ShaderPtr(std::shared_ptr<Shader> shader);

    Shader* operator->(){return &(*_shader.get());}

    ~ShaderPtr();
private:
    std::shared_ptr<Shader> _shader;
};

class ShaderGenerator {
private:
    std::map<std::string, std::shared_ptr<Shader>> _shaders;
    static std::unique_ptr<ShaderGenerator> instance;
    static bool isInit;

public:
    static void Init();
    static ShaderPtr CreateShader(std::string shaderName, std::string vsPath, std::string fsPath, bool isPresistence);
    static void DeleteShader(std::string shaderName);

    static bool IsEmpty() {return instance->_shaders.empty();}
    static void show(){ 
        for(auto& shader : instance->_shaders) {
            std::cout << "shader : " << shader.first << " use_count is " << shader.second.use_count() << std::endl;
        }
    }
    static void Clear();
private:
    ShaderGenerator();
};


class Shader {

private:
    Shader(unsigned int id, std::string name, bool isPresistence);
public:
    static std::unique_ptr<Shader> CreateShader(const char* vertShaderPath, const char* fragShaderPath);
    void Bind();
    void Unbind();

    void ResetLightIdx();
    void SetInt(const std::string&name, int value);
    void SetFloat(const std::string& name, float value);
    void SetMat4f(const std::string& name, glm::mat4& mat);
    void SetVec3f(const std::string& name, const glm::vec3& v);
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

    std::string GetName(){return _name;}
    bool IsPresistence(){return _isPresistence;}
    ~Shader();

    // 友元函数声明
    friend class ShaderGenerator;

public:
    static const char* DiffuseTexturePrefix;
    static const char* SpecularTexturePrefix;

    static const std::string DirLightCnt;
    static const std::string PointLightCnt;
    static const std::string SpotLightCnt;


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
    std::string _name;
    std::unordered_map<std::string, int> _uniformLocation;
    int _directionalLightIdx{0};
    int _pointLightIdx{0};
    int _spotLightIdx{0};
    bool _isPresistence{false};
};


