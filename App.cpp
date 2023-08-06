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
        auto modelShader = ShaderGenerator::CreateShader("modelShader", "./shaders/model_vert.shader", "./shaders/model_frag.shader", false);
        auto outlineShader = ShaderGenerator::CreateShader("outlineShader", "./shaders/outline_vert.shader", "./shaders/outline_frag.shader", true);

        auto modelShader1 = ShaderGenerator::CreateShader("modelShader", "./shaders/model_vert.shader", "./shaders/model_frag.shader", false);
        auto outlineShader1 = ShaderGenerator::CreateShader("outlineShader", "./shaders/outline_vert.shader", "./shaders/outline_frag.shader", true);
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

    glm::vec3 planePosition = glm::vec3(0.0f, -0.5f, 0.0f);
    glm::vec3 cuebPositions[] = {
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(2.0f, 0.0f, 0.0f)
    };

    // 加载贴图
    TextureIndex boxDiffuse;
    boxDiffuse.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    boxDiffuse.idx = TextureManager::LoadTexture("./textures/container.jpg", GL_TEXTURE_2D);


    TextureIndex groundDiffuse;
    groundDiffuse.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    groundDiffuse.idx = TextureManager::LoadTexture("./textures/metal.png", GL_TEXTURE_2D);

    TextureIndex screenTex;
    screenTex.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    screenTex.idx = TextureManager::CreateTexture("framebuffer", GL_TEXTURE_2D, GL_RGB, _width, _height);

    // 创建网格
    auto cube = GeometryGenerator::generateCube();
    cube->SetTexture(boxDiffuse);

    auto plane = GeometryGenerator::generatePlane();
    plane->SetTexture(groundDiffuse);

    auto screenQuad = GeometryGenerator::generateQuad();
    screenQuad->SetTexture(screenTex);

    // 创建着色器
    auto scenseShader = ShaderGenerator::CreateShader("scenseShader" ,"./shaders/framebuffer_vert.shader", "./shaders/framebuffer_frag.shader", true);
    auto framebBufferShader = ShaderGenerator::CreateShader("frameBufferShader", "./shaders/framebuffer_screen_vert.shader", "./shaders/framebuffer_screen_frag.shader", true);

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


    // unsigned int 

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glFrontFace(GL_CCW));

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

        scenseShader->Bind();
        scenseShader->SetMat4f("proj", proj);
        scenseShader->SetMat4f("view", view);

        for (int i = 0; i < sizeof(cuebPositions) / sizeof(glm::vec3); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cuebPositions[i]);

            scenseShader->SetMat4f("model", model);

            cube->Draw(scenseShader);
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, planePosition);
        model = glm::scale(model, glm::vec3(5, 5, 5));
        scenseShader->SetMat4f("model", model);
        plane->Draw(scenseShader);

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


