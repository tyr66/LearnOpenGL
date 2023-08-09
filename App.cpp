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
#include "Input.h"
#include "Camera.h"
#include "light.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "Shader.h"
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
    stbi_set_flip_vertically_on_load(true);

    ShaderGenerator::Init();
    TextureManager::Init();
    MeshManager::Init();

}

void App::clear()
{
    TextureManager::Clear();
    ShaderGenerator::Clear();
    MeshManager::Clear();

}

void App::test()
{
    {
        auto modelShader = ShaderGenerator::CreateShader("modelShader", "./shaders/model_vert.shader", "./shaders/model_frag.shader","", false);
        auto outlineShader = ShaderGenerator::CreateShader("outlineShader", "./shaders/outline_vert.shader", "./shaders/outline_frag.shader", "", true);

        auto modelShader1 = ShaderGenerator::CreateShader("modelShader", "./shaders/model_vert.shader", "./shaders/model_frag.shader", "", false);
        auto outlineShader1 = ShaderGenerator::CreateShader("outlineShader", "./shaders/outline_vert.shader", "./shaders/outline_frag.shader", "", true);
    }

    ShaderGenerator::show();


    {
        auto tex0 = TextureManager::LoadTexture("textures/awesomeface.png", GL_TEXTURE_2D);
        auto tex1 = TextureManager::LoadTexture("textures/container.jpg", GL_TEXTURE_2D);
        auto tex2 = TextureManager::LoadTexture("textures/awesomeface.png", GL_TEXTURE_2D);
        auto tex3 = TextureManager::LoadTexture("textures/container.jpg", GL_TEXTURE_2D);

        assert(tex1 == tex3 && tex0 == tex2);

        std::cout << "texture : " << TextureManager::GetTexture(tex0)->GetName() << ", id = " << tex0 << std::endl;
        std::cout << "texture : " << TextureManager::GetTexture(tex1)->GetName() << ", id = " << tex1 << std::endl;
    }
}

void App::run()
{
    Camera camera;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 skyboxView;

    // 加载天空盒
    std::vector<std::string> cubemapFaces = {
        "./textures/skybox/right.jpg",
        "./textures/skybox/left.jpg",
        "./textures/skybox/top.jpg",
        "./textures/skybox/bottom.jpg",
        "./textures/skybox/front.jpg",
        "./textures/skybox/back.jpg"
    };

    TextureIndex skybox;
    skybox.usage = TextureUsage::TEXTURE_USAGE_SKYBOX;
    skybox.idx = TextureManager::LoadCubeMap("skybox", cubemapFaces);

    TextureIndex screenTex;
    screenTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    screenTex.idx = TextureManager::CreateTexture("framebuffer", GL_TEXTURE_2D, GL_RGB, _width, _height);

    TextureIndex boxTex;
    boxTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    boxTex.idx = TextureManager::LoadTexture("./textures/container.jpg", GL_TEXTURE_2D);


    // 创建网格
    auto skyboxCube = GeometryGenerator::generateSkyBox();

    auto screenQuad = GeometryGenerator::generateQuad();
    screenQuad->SetTexture(screenTex);

    auto box = GeometryGenerator::generateCube();
    box->SetTexture(boxTex);

    // 加载模型
    auto bag = Model::CreateModel("./models/survival-guitar/backpack.obj");
    bag->SetPos(0.0f, 0.0f, -1.0f);

    // 创建着色器
    auto skyboxShader = ShaderGenerator::CreateShader("skyboxShader", "./shaders/skybox_vert.shader", "./shaders/skybox_frag.shader");
    auto scenseShader = ShaderGenerator::CreateShader("scenseShader" ,"./shaders/texture_vert.shader", "./shaders/texture_frag.shader");
    auto framebBufferShader = ShaderGenerator::CreateShader("frameBufferShader", "./shaders/framebuffer_screen_vert.shader", "./shaders/framebuffer_screen_frag.shader");
    auto normalShader = ShaderGenerator::CreateShader("normalShader", "./shaders/normal_vert.shader", "./shaders/normal_frag.shader", "./shaders/normal_ge.shader");

    // 创建framebuffer
    unsigned int fbo;
    GLCall(glGenFramebuffers(1, &fbo));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureManager::GetTextureRenderID(screenTex.idx), 0));

    unsigned int rbo;
    GLCall(glGenRenderbuffers(1, &rbo));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));

    unsigned int flag = GLCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (flag != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[ERROR] failed to create framebuffer, error code is " <<  flag << std::endl;
        throw std::runtime_error("failed to create frambuffer");
    }

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glFrontFace(GL_CCW));

    skyboxShader->Bind();
    skyboxShader->SetGobalTexture("skybox", TextureManager::GetTexture(skybox.idx));

    while(!glfwWindowShouldClose(_window))
    {

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraPos();

        proj = glm::perspective(glm::radians(camera.GetFov()), _width / (float)_height, 0.1f, 100.0f);
        view = camera.GetViewMat4();

        model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        // 绘制天空盒
        GLCall(glDepthFunc(GL_LEQUAL));
        skyboxView = glm::mat4(glm::mat3(view)); // 让skybox永远位于摄像机中心
        skyboxShader->Bind();
        skyboxShader->SetMat4f("view", skyboxView);
        skyboxShader->SetMat4f("proj", proj);
        skyboxCube->Draw(skyboxShader);

        GLCall(glDepthFunc(GL_LESS));

        scenseShader->Bind();
        scenseShader->SetMat4f("proj", proj);
        scenseShader->SetMat4f("view", view);
        scenseShader->SetMat4f("model", model);
        // bag->Draw(scenseShader);
        box->Draw(scenseShader);

        normalShader->Bind();
        normalShader->SetMat4f("proj", proj);
        normalShader->SetMat4f("view", view);
        normalShader->SetMat4f("model", model);
        // bag->Draw(normalShader);
        box->Draw(normalShader);

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glDisable(GL_DEPTH_TEST));
        GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        framebBufferShader->Bind();
        screenQuad->Draw(framebBufferShader);

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


