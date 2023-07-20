#include <iostream>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <glad/glad.h>
#include <memory>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLFW/glfw3.h"
#include "App.h"
#include "help.h"
#include "Shader.h"
#include "IndicesBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Input.h"
#include "Camera.h"
#include "GeometryGenerator.h"


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
    std::vector<float> verts;
    std::vector<unsigned int> indices;
    GeometryGenerator::generateCube(verts, indices, 1.0f, 1.0f, 1.0f);

    Camera camera;

    auto objVbo = VertexBuffer::CreateVertexBuffer((void*)verts.data(), sizeof(float) * verts.size());
    auto objvao = VertexArray::CreateVertexArray();
    auto objEbo = IndiceBuffer::CreateIndiceBuffer(indices.data(), indices.size());
    auto objShader = Shader::CreateShader("../shaders/vert.shader", "../shaders/frag.shader");

    auto lightVbo = VertexBuffer::CreateVertexBuffer((void*)verts.data(), sizeof(float) * verts.size());
    auto lightvao = VertexArray::CreateVertexArray();
    auto lightEbo = IndiceBuffer::CreateIndiceBuffer(indices.data(), indices.size());
    auto lightShader = Shader::CreateShader("../shaders/lightvert.shader", "../shaders/lightfrag.shader");

    VertexBufferLayout objLayout;
    objLayout.push<float>(3);
    objvao->AddBuffer(*objVbo.get(), objLayout);

    VertexBufferLayout lightLayout;
    lightLayout.push<float>(3);
    lightvao->AddBuffer(*lightVbo.get(), lightLayout);
    

    glm::vec3 lightColor = glm::vec3(0.5f, 1.0f, 0.3f);
    glm::vec3 objColor = glm::vec3(1.0f, 0.5f, 0.31f);

    objShader->Bind();
    objShader->SetVec3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
    objShader->SetVec3f("objColor", objColor.x, objColor.y, objColor.z);

    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    lightShader->Bind();
    lightShader->SetVec3f("lightColor", lightColor.x, lightColor.y, lightColor.z);

    // 设置变换矩阵
    glm::mat4 objModel = glm::mat4(1.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    proj = glm::perspective(glm::radians(45.0f), _width / (float)_height, 0.1f, 100.0f);



    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraPos();

        proj = camera.GetProjMat4(_width, _height);
        view = camera.GetViewMat4();

        objModel = glm::mat4(1.0f);
        objModel = glm::translate(objModel, glm::vec3(0, 0, 0));
        objShader->Bind();
        objvao->Bind();
        objEbo->Bind();
        objShader->SetMat4f("view", glm::value_ptr(view));
        objShader->SetMat4f("proj", glm::value_ptr(proj));
        objShader->SetMat4f("model", glm::value_ptr(objModel));
        GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));


        lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
        lightvao->Bind();
        lightShader->Bind();
        lightEbo->Bind();
        lightShader->SetMat4f("view", glm::value_ptr(view));
        lightShader->SetMat4f("proj", glm::value_ptr(proj));
        lightShader->SetMat4f("model", glm::value_ptr(lightModel));
        GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));

        processInput();

        Input::Reset();
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
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("failed to init GLAD");
    }

    Input::Init(_window);

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


