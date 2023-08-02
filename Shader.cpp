#include <fstream>
#include <sstream>


#include "glad/glad.h"
#include "help.h"
#include "Shader.h"
#include "light.h"

const char* Shader::DiffuseTexturePrefix = "texture_diffuse";
const char* Shader::SpecularTexturePrefix = "texture_specular";

static std::string readFile(const char* filePath)
{
    std::stringstream ss;
    std::ifstream file;
    std::string msg;
    size_t fileSz;
    std::string line;

    file.exceptions(std::ifstream::badbit);

    try
    {
        file.open(filePath, std::ios_base::ate);
        fileSz = file.tellg();
        file.seekg(std::ios_base::beg);
        
        while (std::getline(file, line)) {
            ss << line << '\n';
        }

        file.close();

    } catch(std::ifstream::failure e) {

        if (!file.is_open()) 
            file.close();

        std::cout << "read file " << std::endl << ss.str() << std::endl;
        throw e;
    }

    // std::cout << "read file " << std::endl << ss.str() << std::endl;
    return ss.str();
}

static unsigned int compileShader(unsigned int shaderType, const char* filePath){

    char* logInfo;
    unsigned int shader;
    auto shaderSource = readFile(filePath);
    const char* source = shaderSource.c_str();
    //std::cout << source << std::endl << "-----------------------------------------" << std::endl;
    shader = GLCall(glCreateShader(shaderType));
    GLCall(glShaderSource(shader, 1, &source, nullptr));
    GLCall(glCompileShader(shader));
    int successed;
    GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &successed));

    if (!successed) {
        int length;
        GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
        logInfo = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(shader, length, &length, logInfo));
        std::cout << logInfo << std::endl;
        throw std::runtime_error(logInfo);
    }

    return shader;
}

Shader::Shader(unsigned int id):_renderID(id)
{
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(_renderID));
}

std::unique_ptr<Shader> Shader::CreateShader(const char* vertShaderPath, const char* fragShaderPath)
{
    char* infoLog;
    unsigned int shaderID;
    unsigned int vertexShader = 0, fragmentShader = 0;

    try{

        vertexShader = compileShader(GL_VERTEX_SHADER, vertShaderPath);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragShaderPath);
        shaderID = GLCall(glCreateProgram());

        GLCall(glAttachShader(shaderID, vertexShader));
        GLCall(glAttachShader(shaderID, fragmentShader));
        glLinkProgram(shaderID);
        int successed;
        GLCall(glGetProgramiv(shaderID, GL_LINK_STATUS, &successed));
        if (!successed) {
            int length;
            GLCall(glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &length));
            infoLog = (char*)alloca(length * sizeof(char));
            GLCall(glGetProgramInfoLog(shaderID, length, &length, infoLog));
            throw std::runtime_error(infoLog);
        }
    } catch(std::exception& e)
    {
        if (vertexShader != 0)
            GLCall(glDeleteShader(vertexShader));
        if (fragmentShader != 0)
            GLCall(glDeleteShader(fragmentShader));
        std::cout << e.what() << std::endl;
        throw e;
    }

    GLCall(glDeleteShader(vertexShader));
    GLCall(glDeleteShader(fragmentShader));

    return std::unique_ptr<Shader>(new Shader(shaderID));
}


void Shader::Bind()
{
    GLCall( glUseProgram(_renderID));
}

void Shader::Unbind()
{
    GLCall(glUseProgram(0));
}

void Shader::ResetLightIdx()
{
    _directionalLightIdx = 0;
    _pointLightIdx = 0;
    _spotLightIdx = 0;
}

std::string Shader::GetDirectionLightPrefix(int idx)
{
    return "dirLights[" + std::to_string(idx) + "].";
}

std::string Shader::GetPointLightPrefix(int idx)
{
    return "pointLights[" + std::to_string(idx) + "].";
}

std::string Shader::GetSpotLightPrefix(int idx)
{
    return "spotLights[" + std::to_string(idx) + "].";
}

void Shader::SetInt(std::string& name, int value)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform1i(location, value));
}

void Shader::SetFloat(std::string& name, float value)
{

    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform1f(location, value));
}

void Shader::SetInt(std::string&& name, int value)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform1i(location, value));
}

void Shader::SetFloat(std::string&& name, float value)
{

    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform1f(location, value));
}


void Shader::SetMat4f(std::string&name, const float* data)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, data));
}

void Shader::SetMat4f(std::string&&name, const float*data)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, data));
}

void Shader::SetVec3f(std::string& name, const glm::vec3& v)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform3f(location, v.x, v.y, v.z));
}

void Shader::SetVec3f(std::string&& name, const glm::vec3& v)
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }
    GLCall(glUniform3f(location, v.x, v.y, v.z));
}

void Shader::SetDirectionalLight(const DirectionalLight& light)
{
    std::string prefix = this->GetDirectionLightPrefix(_directionalLightIdx++);

    int location = getAndSetLocation(prefix + "lightDir");
    GLCall(glUniform3f(location, light.lightDir.x, light.lightDir.y, light.lightDir.z));

    location = getAndSetLocation(prefix + "ambient");
    GLCall(glUniform3f(location, light.ambient.x, light.ambient.y, light.ambient.z));


    location = getAndSetLocation(prefix + "diffuse");
    GLCall(glUniform3f(location, light.diffuse.x, light.diffuse.y, light.diffuse.z));

    location = getAndSetLocation(prefix + "specular");
    GLCall(glUniform3f(location, light.specular.x, light.specular.y, light.specular.z));

}
void Shader::SetSpotLight(const SpotLight& light)
{
    std::string prefix = GetSpotLightPrefix(_spotLightIdx++);

    int location = getAndSetLocation(prefix + "lightDir");
    GLCall(glUniform3f(location, light.lightDir.x, light.lightDir.y, light.lightDir.z));

    location = getAndSetLocation(prefix + "lightPos");
    GLCall(glUniform3f(location, light.lightPos.x, light.lightPos.y, light.lightPos.z));

    location = getAndSetLocation(prefix + "ambient");
    GLCall(glUniform3f(location, light.ambient.x, light.ambient.y, light.ambient.z));


    location = getAndSetLocation(prefix + "diffuse");
    GLCall(glUniform3f(location, light.diffuse.x, light.diffuse.y, light.diffuse.z));

    location = getAndSetLocation(prefix + "specular");
    GLCall(glUniform3f(location, light.specular.x, light.specular.y, light.specular.z));


    location = getAndSetLocation(prefix + "outerCutOff");
    GLCall(glUniform1f(location, light.outerCutoff));


    location = getAndSetLocation(prefix + "cutOff");
    GLCall(glUniform1f(location, light.cutOff));

    location = getAndSetLocation(prefix + "constantAttenuation");
    GLCall(glUniform1f(location, light.constant));

    location = getAndSetLocation(prefix + "linearAttenuation");
    GLCall(glUniform1f(location, light.linear));

    location = getAndSetLocation(prefix + "quadraticAttenuation");
    GLCall(glUniform1f(location, light.quadratic));
}
void Shader::SetPointLight(const PointLight& light)
{
    std::string prefix = GetPointLightPrefix(_pointLightIdx++);

    int location = getAndSetLocation(prefix + "lightPos");
    GLCall(glUniform3f(location, light.lightPos.x, light.lightPos.y, light.lightPos.z));

    location = getAndSetLocation(prefix + "ambient");
    GLCall(glUniform3f(location, light.ambient.x, light.ambient.y, light.ambient.z));


    location = getAndSetLocation(prefix + "diffuse");
    GLCall(glUniform3f(location, light.diffuse.x, light.diffuse.y, light.diffuse.z));

    location = getAndSetLocation(prefix + "specular");
    GLCall(glUniform3f(location, light.specular.x, light.specular.y, light.specular.z));

    location = getAndSetLocation(prefix + "constantAttenuation");
    GLCall(glUniform1f(location, light.constant));

    location = getAndSetLocation(prefix + "linearAttenuation");
    GLCall(glUniform1f(location, light.linear));

    location = getAndSetLocation(prefix + "quadraticAttenuation");
    GLCall(glUniform1f(location, light.quadratic));
}

void Shader::SetDirectionalLightNum(int num)
{
    this->SetInt("dirLightCnt", num);
}

void Shader::SetPointLightNum(int num)
{
    this->SetInt("pointLightCnt", num);
}

void Shader::SetSpotLightNum(int num)
{
    this->SetInt("spotLightCnt", num);
}

int Shader::getAndSetLocation(const std::string& name) 
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }

    return location;
}

int Shader::getAndSetLocation(const std::string&& name) 
{
    int location;
    if (_uniformLocation.find(name) == _uniformLocation.end()) {
        location = GLCall(glGetUniformLocation(_renderID, name.c_str()));
        _uniformLocation[name] = location;
    }
    else {
        location = _uniformLocation[name];
    }

    return location;
}
