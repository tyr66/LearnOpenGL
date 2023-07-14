#include <iostream>
#include <fstream>
#include <stdexcept>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "App.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

App::App(int width, int height): _width(width), _height(height)
{

}

App::~App()
{
    if (_window != nullptr) {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    if (_initShader) {
        glDeleteProgram(_shaderProgram);
    }

    if (_initBuffer) {
        glDeleteBuffers(1,  &_VBO);
        glDeleteVertexArrays(1, &_VAO);
    }

    glfwTerminate();
}

void App::init()
{
    initGlfw();
    initShader();
    initBuffer();
}

void App::run()
{

    while(!glfwWindowShouldClose(_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        processInput();

        glfwPollEvents();
        glfwSwapBuffers(_window);
    }
}

void App::initGlfw()
{

    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("failed to init glfw library");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(_width, _height, "Start", nullptr, nullptr);

    if (_window == nullptr) {
        throw std::runtime_error("failed to create glfw Window");
    }

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("failed to init GLAD");
    }

    std::cout << "init glfw successed" << std::endl;
}
void App::initBuffer()
{
    float pos[] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    _initBuffer = true;

}

std::vector<char> App::readFile(const char* filePath)
{
    std::vector<char> res;
    std::ifstream file(filePath, std::ios_base::ate);
    std::string msg;

    if (!file.is_open()) {
        msg += "faile to read file , path = "; msg += filePath;
        throw std::runtime_error(msg);
    }

    size_t fileSz = file.tellg();
    file.seekg(std::ios_base::beg);
    res.resize(fileSz);

    file.read(res.data(), fileSz);
    return res;
}
unsigned int App::compileShader(unsigned int shaderType, const char* filePath){

    char* logInfo;
    unsigned int shader;
    auto shaderSource = readFile(filePath);
    // std::cout << "read file " << std::endl << shaderSource.data() << std::endl;
    const char* source = shaderSource.data();
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int successed;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &successed);

    if (!successed) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        logInfo = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, logInfo);
        throw std::runtime_error(logInfo);
    }

    return shader;
}

void App::initShader()
{
    char* infoLog;
    unsigned int vertShader = compileShader(GL_VERTEX_SHADER, "../vert.shader");
    unsigned int fragShader = compileShader(GL_FRAGMENT_SHADER, "../frag.shader");
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertShader);
    glAttachShader(_shaderProgram, fragShader);
    glLinkProgram(_shaderProgram);

    int successed;
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &successed);
    if (!successed) {
        int length;
        glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &length);
        infoLog = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(_shaderProgram, length, &length, infoLog);
        throw std::runtime_error(infoLog);
    }

    glUseProgram(_shaderProgram);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    _initShader = true;
}

void App::processInput()
{
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);
}


