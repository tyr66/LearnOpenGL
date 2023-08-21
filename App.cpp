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
    //stbi_set_flip_vertically_on_load(true);

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


    // 创建贴图
    TextureIndex framebufferTex;
    framebufferTex.usage = TextureUsage::TEXTURE_USAGE_FRAMEBUFFER;
    framebufferTex.idx = TextureManager::CreateTexture("multiSampler",GL_TEXTURE_2D_MULTISAMPLE, GL_RGB, _width, _height, GL_UNSIGNED_BYTE);

    TextureIndex intermediateTex;
    intermediateTex.usage = TextureUsage::TEXTURE_USAGE_FRAMEBUFFER;
    intermediateTex.idx = TextureManager::CreateTexture("intermediate", GL_TEXTURE_2D, GL_RGB, _width, _height, GL_UNSIGNED_BYTE);


    TextureIndex wallTex;
    wallTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    wallTex.idx = TextureManager::LoadTexture("./textures/metal.png", GL_TEXTURE_2D);

    TextureIndex planeTex;
    planeTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    planeTex.idx = TextureManager::LoadTexture("./textures/bricks/wood.png", GL_TEXTURE_2D);
    TextureIndex planeNormalTex;
    planeNormalTex.usage = TextureUsage::TEXTURE_USAGE_NORMAL;
    planeNormalTex.idx = TextureManager::LoadTexture("./textures/bricks/toy_box_normal.png", GL_TEXTURE_2D);
    TextureIndex planeParallaxTex;
    planeParallaxTex.usage = TextureUsage::TEXTURE_USAGE_PARALLAX;
    planeParallaxTex.idx = TextureManager::LoadTexture("./textures/bricks/toy_box_disp.png", GL_TEXTURE_2D);


    TextureIndex screenTex;
    screenTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    screenTex.idx = intermediateTex.idx;

    TextureIndex shadowTex;
    shadowTex.usage = TextureUsage::TEXTURE_USAGE_SHADOWCUBEMAP;
    shadowTex.idx = TextureManager::CreateTexture("shadowCubeMap", GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight, GL_FLOAT);
    auto shadowTexPtr = TextureManager::GetTexture(shadowTex.idx);

    // 创建网格
    auto screenQuad = GeometryGenerator::generateQuad();
    screenQuad->SetTexture(screenTex);

    auto wallMesh = GeometryGenerator::generateInverseCube();
    wallMesh->SetTexture(wallTex);
    auto wall = Model::CreateModel({wallMesh});
    wall->SetScale(10.0f, 10.0, 10.0f);
    wall->SetPos(0.0f, 0.0f, 0.0f);

    auto planeMesh = GeometryGenerator::generatePlane(true);
    planeMesh->SetTexture(planeTex);
    planeMesh->SetTexture(planeNormalTex);
    planeMesh->SetTexture(planeParallaxTex);
    auto plane = Model::CreateModel({planeMesh});
    plane->SetPos(0, 0, -4);
    plane->SetRotation(90, 0, 0);

    // 创建着色器
    auto framebBufferShader = ShaderGenerator::CreateShader("frameBufferShader", "./shaders/framebuffer_screen_vert.shader", "./shaders/framebuffer_screen_frag.shader");
    auto planeShader = ShaderGenerator::CreateShader("planeShader", "./shaders/normal_parallax_vert.shader", "./shaders/normal_parallax_frag.shader");
    auto shadowMapShader = ShaderGenerator::CreateShader("shadowMapShader", "./shaders/shadow_tex_vert.shader", "./shaders/shadow_tex_frag.shader", "./shaders/shadow_tex_geo.shader");
    auto wallShader = ShaderGenerator::CreateShader("wallShader", "./shaders/shadow_map_vert.shader", "./shaders/shadow_map_frag.shader");

    // 创建framebuffer
    unsigned int flag = 0;

    unsigned int shadowFbo;
    GLCall(glGenFramebuffers(1, &shadowFbo));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo));
    GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexPtr->GetRenderID(), 0));
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

    PointLight light;
    light.lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;
    light.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // point shadows 每一个面的阴影变换矩阵
    float near_plane  = 1.0f;
    float far_plane = 25.0f;
    glm::mat4 lightProjMat = glm::perspective(glm::radians(90.0f), (float)_shadowWidth / _shadowHeight, near_plane, far_plane);
    glm::mat4 shadowTransformMats[] = {
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjMat * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    };

    shadowMapShader->Bind();
    shadowMapShader->SetFloat("far_plane", far_plane);
    shadowMapShader->SetVec3f("lightPos", light.lightPos);
    for (int i = 0; i < sizeof(shadowTransformMats) / sizeof(glm::mat4); i++) {
        std::string name = "shadowTransformMats[" + std::to_string(i) + "]";
        shadowMapShader->SetMat4f(name, shadowTransformMats[i]);
    }

    planeShader->Bind();
    planeShader->SetPointLight(light);
    planeShader->SetPointLightNum(1);
    planeShader->SetFloat("far_plane", far_plane);

    wallShader->Bind();
    wallShader->SetPointLightNum(1);
    wallShader->SetPointLight(light);
    wallShader->SetFloat("far_plane", far_plane);


    while(!glfwWindowShouldClose(_window))
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo));
        GLCall(glViewport(0, 0, _shadowWidth, _shadowHeight));

        // 绘制墙壁
        GLCall(glDisable(GL_CULL_FACE));
        wall->Draw(shadowMapShader);
        GLCall(glEnable(GL_CULL_FACE));

        plane->Draw(shadowMapShader);


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

        planeShader->Bind();
        planeShader->SetTexture("shadow_texture", shadowTexPtr);
        planeShader->SetMat4f("view", view);
        planeShader->SetMat4f("proj", proj);
        planeShader->SetVec3f("viewPos", camera.GetPos());

        wallShader->Bind();
        wallShader->SetTexture("shadow_texture", shadowTexPtr);
        wallShader->SetMat4f("view", view);
        wallShader->SetMat4f("proj", proj);
        wallShader->SetVec3f("viewPos", camera.GetPos());

        GLCall(glDisable(GL_CULL_FACE));
        wall->Draw(wallShader);
        GLCall(glEnable(GL_CULL_FACE));

        plane->Draw(planeShader);


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


