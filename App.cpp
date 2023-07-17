#include <iostream>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <glad/glad.h>
#include <memory>
#include <stb_image.h>

#include "GLFW/glfw3.h"
#include "App.h"
#include "help.h"
#include "Shader.h"
#include "IndicesBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"


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

    stbi_set_flip_vertically_on_load(true);
}

void App::run()
{
    float pos[] = {
// positions          // colors           // texture coords
     0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,   2.0f, 2.0f,   // top right
     0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  2.0f, 0.0f,   // bottom right
    -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 2.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    auto vbo = VertexBuffer::CreateVertexBuffer((void*)pos, sizeof(pos));
    auto ebo = IndiceBuffer::CreateIndiceBuffer(indices, sizeof(indices) / sizeof(unsigned int));
    auto vao = VertexArray::CreateVertexArray();
    auto shader = Shader::CreateShader("../vert.shader", "../frag.shader");

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(4);
    layout.push<float>(2);
    vao->AddBuffer(*vbo.get(), layout);
    
    auto texture1 = Texture::CreateTexture("../textures/container.jpg", GL_TEXTURE_2D, GL_RGB);
    texture1->SetWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture1->SetFiltering(GL_LINEAR, GL_LINEAR);

    auto texture2 = Texture::CreateTexture("../textures/awesomeface.png", GL_TEXTURE_2D, GL_RGBA);
    texture2->SetWrapping(GL_REPEAT, GL_REPEAT);
    texture2->SetFiltering(GL_LINEAR, GL_LINEAR);

    vao->Bind();
    ebo->Bind();
    shader->Bind();
    shader->SetInt("texture1", 0);
    shader->SetInt("texture2", 1);

    texture2->BindAndActive(GL_TEXTURE1);
    texture1->BindAndActive(GL_TEXTURE0);

    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));


        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

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


