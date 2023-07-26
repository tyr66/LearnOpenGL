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
#include "light.h"


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

    auto diffuseTex = Texture::CreateTexture("../textures/container2.png", GL_TEXTURE_2D, GL_RGB);
    diffuseTex->SetFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    diffuseTex->SetWrapping(GL_REPEAT, GL_REPEAT);
    auto specularTex = Texture::CreateTexture("../textures/container2_specular.png", GL_TEXTURE_2D, GL_RGB);
    specularTex->SetFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    specularTex->SetWrapping(GL_REPEAT, GL_REPEAT);

    VertexBufferLayout objLayout;
    objLayout.push<float>(3);
    objLayout.push<float>(3);
    objLayout.push<float>(2);
    objvao->AddBuffer(*objVbo.get(), objLayout);

    VertexBufferLayout lightLayout;
    lightLayout.push<float>(3);
    lightLayout.push<float>(3);
    lightLayout.push<float>(2);
    lightvao->AddBuffer(*lightVbo.get(), lightLayout);
    
glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };


    glm::vec3 objColor = glm::vec3(1.0f, 0.5f, 0.31f);

    objShader->Bind();
    objShader->SetVec3f("objColor", objColor);
    objShader->SetInt("material.diffuse", 0);
    objShader->SetInt("material.specular", 1);
    diffuseTex->BindAndActive(GL_TEXTURE0);
    specularTex->BindAndActive(GL_TEXTURE1);

    lightShader->Bind();

    // 设置变换矩阵
    glm::mat4 objModel = glm::mat4(1.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    proj = glm::perspective(glm::radians(45.0f), _width / (float)_height, 0.1f, 100.0f);

    Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.shiness = 64.0f;

    DirectionalLight dirLight;
    dirLight.lightDir = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    dirLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    dirLight.name = "dirLight";

    SpotLight spotLight;
    spotLight.name = "spotLight";
    spotLight.lightDir = glm::vec3(0.0f, 0.0f, -1.0f);
    spotLight.lightPos = glm::vec3(0.0f);
    spotLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.cutOff = glm::cos(glm::radians(12.5f));
    spotLight.outerCutoff = glm::cos(glm::radians(15.0f));
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;

    PointLight pointLights[4];
    for (int i = 0; i < 4; i++) {
        pointLights[i].name = "pointLights[" + std::to_string(i) + ']';
        pointLights[i].lightPos = pointLightPositions[i];
        pointLights[i].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        pointLights[i].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        pointLights[i].specular = glm::vec3(1.0f, 1.0f, 1.0f);
        pointLights[i].constant = 1.0f;
        pointLights[i].linear = 0.09f;
        pointLights[i].quadratic = 0.032f;
        // std::cout << pointLights[i].name << std::endl;
    }

    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        // GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraPos();

        proj = camera.GetProjMat4(_width, _height);
        view = camera.GetViewMat4();
        glm::vec3 viewPos = camera.GetPos();

        spotLight.lightPos = camera.GetPos();
        spotLight.lightDir = -camera.GetFront();

        objModel = glm::mat4(1.0f);
        objModel = glm::translate(objModel, glm::vec3(0, 0, 0));
        objShader->Bind();
        objvao->Bind();
        objEbo->Bind();
        objShader->SetMat4f("view", glm::value_ptr(view));
        objShader->SetMat4f("proj", glm::value_ptr(proj));
        objShader->SetVec3f("viewPos", viewPos);
        objShader->SetDirectionalLight(dirLight);
        objShader->SetSpotLight(spotLight);
        objShader->SetInt("pointLightCnt", 4);
        for (int i = 0; i < 4; i++) {
            objShader->SetPointLight(pointLights[i]);
        }

        objShader->SetFloat("material.shiness", material.shiness);

        for (int i = 0; i < 10; i++)
        {
            objModel = glm::mat4(1.0f);
            objModel = glm::translate(objModel,cubePositions[i]);
            objModel = glm::rotate(objModel, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            objShader->SetMat4f("model", glm::value_ptr(objModel));
            GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
        }


        // 绘制灯光物体
        lightvao->Bind();
        lightShader->Bind();
        lightEbo->Bind();
        lightShader->SetMat4f("view", glm::value_ptr(view));
        lightShader->SetMat4f("proj", glm::value_ptr(proj));

        for (int i = 0; i < 4; i++)
        {
            lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, pointLightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f, 0.2f, 0.2f));
            lightShader->SetMat4f("model", glm::value_ptr(lightModel));
            GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
        }

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


