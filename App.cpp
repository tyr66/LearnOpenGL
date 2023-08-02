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
#include "Input.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "light.h"
#include "Model.h"


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
    glm::vec3 lightDir = glm::vec3(0.5f, -1.0f, -1.0f);
    DirectionalLight dirLight;
    dirLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.diffuse = glm::vec3(0.33f, 0.33f, 0.33f);
    dirLight.specular = glm::vec3(0.44f, 0.44f, 0.44f);

    SpotLight spotLight;
    spotLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;
    spotLight.cutOff = glm::cos(glm::radians(12.5f));
    spotLight.outerCutoff = glm::cos(glm::radians(20.0f));

    auto bag = Model::CreateModel("./models/survival-guitar/backpack.obj");
    bag->SetScale(0.5f, 0.5f, 0.5f);
    bag->SetPos(0.0f, 0.0f, -1.0f);

    Camera camera;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.SetCamera(pos, front, up, 0.1f , 100.0f);

    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;

    auto bagShader = Shader::CreateShader("./shaders/model_vert.shader", "./shaders/depth_frag.shader");
    bagShader->Bind();

    bagShader->SetDirectionalLightNum(1);
    bagShader->SetSpotLightNum(1);
    bagShader->SetPointLightNum(0);
    bagShader->SetFloat("near", camera.GetNear());
    bagShader->SetFloat("far", camera.GetFar());

    while(!glfwWindowShouldClose(_window))
    {

        GLCall(glEnable(GL_DEPTH_TEST));
        //GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraPos();

        model = bag->GetModelMat();
        proj = glm::perspective(glm::radians(camera.GetFov()), _width / (float)_height, 0.1f, 100.0f);
        view = camera.GetViewMat4();

        dirLight.lightDir = glm::normalize(view * glm::vec4(lightDir, 0.0f));
        spotLight.lightPos = view * glm::vec4(camera.GetPos(), 1.0f);
        spotLight.lightDir = glm::normalize(view * glm::vec4(camera.GetFront(), 0.0f));


        bagShader->SetDirectionalLight(dirLight);
        bagShader->SetSpotLight(spotLight);

        bagShader->SetMat4f("model", glm::value_ptr(model));
        bagShader->SetMat4f("proj", glm::value_ptr(proj));
        bagShader->SetMat4f("view", glm::value_ptr(view));

        bag->Draw(*bagShader.get());

        //camera.PrintfInfo();

        bagShader->ResetLightIdx(); // 重设光照的索引

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


