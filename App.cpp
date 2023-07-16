#include <iostream>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <glad/glad.h>
#include <memory>

#include "GLFW/glfw3.h"
#include "App.h"
#include "help.h"
#include "Shader.h"
#include "IndicesBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"


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
    float pos[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
    };

    auto vbo = VertexBuffer::CreateVertexBuffer((void*)pos, sizeof(pos));
    auto ebo = IndiceBuffer::CreateIndiceBuffer(indices, sizeof(indices) / sizeof(unsigned int));
    auto vao = VertexArray::CreateVertexArray();
    auto shader = Shader::CreateShader("../vert.shader", "../frag.shader");

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(4);
    vao->AddBuffer(*vbo.get(), layout);
    
    shader->Bind();
    vao->Bind();
    ebo->Bind();

    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

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

}

void App::initShader()
{
}

void App::processInput()
{
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);
}


