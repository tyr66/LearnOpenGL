#include <fstream>
#include <string>

#include "glad/glad.h"
#include "help.h"
#include "Shader.h"


static std::vector<char> readFile(const char* filePath)
{
    std::vector<char> res;
    std::ifstream file;
    std::string msg;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(filePath, std::ios_base::ate);
        size_t fileSz = file.tellg();
        file.seekg(std::ios_base::beg);
        res.resize(fileSz);
        file.read(res.data(), fileSz);

    } catch(std::ifstream::failure e) {
        msg += "faile to read file , path = "; msg += filePath;
        std::runtime_error(e.what());
    }

    return res;
}

static unsigned int compileShader(unsigned int shaderType, const char* filePath){

    char* logInfo;
    unsigned int shader;
    auto shaderSource = readFile(filePath);
    // std::cout << "read file " << std::endl << shaderSource.data() << std::endl;
    const char* source = shaderSource.data();
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
