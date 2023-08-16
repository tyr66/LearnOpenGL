#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
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

void App::renderScense(ShaderPtr& ahder)
{

}

void App::run()
{
    Camera camera;
    glm::mat4 proj;
    glm::mat4 view;

    TextureIndex framebufferTex;
    framebufferTex.usage = TextureUsage::TEXTURE_USAGE_FRAMEBUFFER;
    framebufferTex.idx = TextureManager::CreateTexture("multiSampler",GL_TEXTURE_2D_MULTISAMPLE, GL_RGB, _width, _height, GL_UNSIGNED_BYTE);

    TextureIndex intermediateTex;
    intermediateTex.usage = TextureUsage::TEXTURE_USAGE_FRAMEBUFFER;
    intermediateTex.idx = TextureManager::CreateTexture("intermediate", GL_TEXTURE_2D, GL_RGB, _width, _height, GL_UNSIGNED_BYTE);

    TextureIndex boxTex;
    boxTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    boxTex.idx = TextureManager::LoadTexture("./textures/container.jpg", GL_TEXTURE_2D);

    TextureIndex floorTex;
    floorTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    floorTex.idx = TextureManager::LoadTexture("./textures/metal.png", GL_TEXTURE_2D);

    TextureIndex screenTex;
    screenTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    screenTex.idx = intermediateTex.idx;

    TextureIndex shadowTex;
    shadowTex.usage = TextureUsage::TEXTURE_USAGE_SHADOWMAP;
    shadowTex.idx = TextureManager::CreateTexture("shadowMap", GL_TEXTURE_2D, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight, GL_FLOAT);
    auto shadowTexPtr = TextureManager::GetTexture(shadowTex.idx);
    shadowTexPtr->SetWrapping(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    shadowTexPtr->SetFiltering(GL_NEAREST, GL_NEAREST);
    float borderColor[]  = {1.0, 1.0, 1.0, 1.0};
    shadowTexPtr->SetBorderColor(borderColor);

    // 创建网格
    auto screenQuad = GeometryGenerator::generateQuad();
    screenQuad->SetTexture(screenTex);

    auto boxMesh = GeometryGenerator::generateCube();
    boxMesh->SetTexture(boxTex);
    auto box = Model::CreateModel({boxMesh});

    auto floorMesh = GeometryGenerator::generatePlane();
    floorMesh->SetTexture(floorTex);
    auto floor = Model::CreateModel({floorMesh});
    floor->SetScale(10.0f, 10.0, 10.0f);
    floor->SetPos(0.0f, -0.5f, 0.0f);

    // 创建着色器
    auto framebBufferShader = ShaderGenerator::CreateShader("frameBufferShader", "./shaders/framebuffer_screen_vert.shader", "./shaders/framebuffer_screen_frag.shader");
    auto sceneShader = ShaderGenerator::CreateShader("boxShader", "./shaders/shadow_map_vert.shader", "./shaders/shadow_map_frag.shader");
    auto shadowMapShader = ShaderGenerator::CreateShader("shadowMapShader", "./shaders/shadow_tex_vert.shader", "./shaders/shadow_tex_frag.shader");

    // 创建framebuffer
    unsigned int flag = 0;

    unsigned int shadowFbo;
    GLCall(glGenFramebuffers(1, &shadowFbo));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexPtr->GetRenderID(), 0));
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    flag = GLCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (flag != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[ERROR] failed to create shadowMap framebuffer, error code is " <<  flag << std::endl;
        throw std::runtime_error("failed to create shadowmap frambuffer");
    }

    unsigned int fbo;
    unsigned int renderid = TextureManager::GetTexture(framebufferTex.idx)->GetRenderID();
    GLCall(glGenFramebuffers(1, &fbo));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE
                                  , renderid
                                  , 0));

    unsigned int rbo;
    GLCall(glGenRenderbuffers(1, &rbo));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, _width, _height));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));

    flag = GLCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (flag != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[ERROR] failed to create framebuffer, error code is " <<  flag << std::endl;
        throw std::runtime_error("failed to create frambuffer");
    }
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    // 创建第二个fbo, 用于进行贴图还原
    unsigned int intermediateFBO;
    renderid = TextureManager::GetTexture(intermediateTex.idx)->GetRenderID();
    GLCall(glGenFramebuffers(1, &intermediateFBO));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D
									, renderid
                                  , 0));

    flag = GLCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (flag != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[ERROR] failed to create intermediate framebuffer, error code is " <<  flag << std::endl;
        throw std::runtime_error("failed to create frambuffer");
    }
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glFrontFace(GL_CCW));

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
    glm::mat4 lightProj, lightView;
    lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 20.0f);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMat = lightProj * lightView;

    DirectionalLight light;
    light.lightDir = glm::normalize(-lightPos);
    light.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    shadowMapShader->Bind();
    shadowMapShader->SetMat4f("lightSpaceMat", lightSpaceMat);

    sceneShader->Bind();
    sceneShader->SetDirectionalLight(light);
    sceneShader->SetDirectionalLightNum(1);

    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo));
        GLCall(glViewport(0, 0, _shadowWidth, _shadowHeight));

        floor->Draw(shadowMapShader);
        box->SetPos(0.0f, 1.5f, 0.0f);
        box->Draw(shadowMapShader);
        box->SetPos(2.0f, 0.0f, 1.0f);
        box->Draw(shadowMapShader);
        box->SetPos(-1.0f, 0.0f, 2.0f);
        box->Draw(shadowMapShader);


        GLCall(glViewport(0, 0, _width, _height));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraPos();

        proj = glm::perspective(glm::radians(camera.GetFov()), _width / (float)_height, 0.1f, 100.0f);
        view = camera.GetViewMat4();

        sceneShader->Bind();
        sceneShader->SetTexture("shadow_texture", shadowTexPtr);
        sceneShader->SetMat4f("view", view);
        sceneShader->SetMat4f("proj", proj);
        sceneShader->SetMat4f("lightSpaceMat", lightSpaceMat);
        sceneShader->SetVec3f("viewPos", camera.GetPos());

        floor->Draw(sceneShader);
        box->SetPos(0.0f, 1.5f, 0.0f);
        box->Draw(sceneShader);
        box->SetPos(2.0f, 0.0f, 1.0f);
        box->Draw(sceneShader);
        box->SetPos(-1.0f, 0.0f, 2.0f);
        box->Draw(sceneShader);

        GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO));
        GLCall(glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

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


