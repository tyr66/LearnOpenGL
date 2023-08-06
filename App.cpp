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
    TextureGenerator::Init();
    MeshManager::Init();

}

void App::clear()
{
    TextureGenerator::Clear();
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
        auto tex0 = TextureGenerator::LoadTexture("textures/awesomeface.png", GL_TEXTURE_2D);
        auto tex1 = TextureGenerator::LoadTexture("textures/container.jpg", GL_TEXTURE_2D);
        auto tex2 = TextureGenerator::LoadTexture("textures/awesomeface.png", GL_TEXTURE_2D);
        auto tex3 = TextureGenerator::LoadTexture("textures/container.jpg", GL_TEXTURE_2D);

        assert(tex1 == tex3 && tex0 == tex2);

        std::cout << "texture : " << TextureGenerator::GetTexture(tex0)->GetName() << ", id = " << tex0 << std::endl;
        std::cout << "texture : " << TextureGenerator::GetTexture(tex1)->GetName() << ", id = " << tex1 << std::endl;
    }
}

void App::run()
{
    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  -0.5f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
    };

    glm::vec3 windowsPositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(-1.0f, 0.0f, 1.0f),
    };


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

    auto window = GeometryGenerator::generateWindowQuad();

    Camera camera;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.SetCamera(pos, front, up, 0.1f , 100.0f);

    glm::mat4 view;
    glm::mat4 proj;

    auto modelShader = ShaderGenerator::CreateShader("bagShader", "./shaders/model_vert.shader", "./shaders/model_frag.shader", false);
    modelShader->Bind();
    modelShader->SetDirectionalLightNum(1);
    modelShader->SetSpotLightNum(1);
    modelShader->SetPointLightNum(0);

    auto outlineShader = ShaderGenerator::CreateShader("outlineShader", "./shaders/outline_vert.shader", "./shaders/outline_frag.shader", true);
    std::map<float, glm::vec3> renderOrder;
    //bag->SetOutline(true);
    
    auto transparentShader = ShaderGenerator::CreateShader("transparentShader", "./shaders/transparent_vert.shader", "./shaders/transparent_frag.shader", true);
    transparentShader->Bind();
    transparentShader->SetDirectionalLightNum(1);
    transparentShader->SetSpotLightNum(1);
    transparentShader->SetPointLightNum(0);

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));

    while(!glfwWindowShouldClose(_window))
    {

        GLCall(glEnable(GL_DEPTH_TEST));
        //GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        camera.UpdateCameraFov(Input::getXoffset(), Input::getYoffste());
        camera.UpdateCameraFront(Input::getXMouse(), Input::getYMouse());
        camera.UpdateCameraPos();

        proj = glm::perspective(glm::radians(camera.GetFov()), _width / (float)_height, 0.1f, 100.0f);
        view = camera.GetViewMat4();

        dirLight.lightDir = glm::normalize(view * glm::vec4(lightDir, 0.0f));
        spotLight.lightPos = view * glm::vec4(camera.GetPos(), 1.0f);
        spotLight.lightDir = glm::normalize(view * glm::vec4(camera.GetFront(), 0.0f));

        modelShader->Bind();
        modelShader->SetDirectionalLight(dirLight);
        modelShader->SetSpotLight(spotLight);

        modelShader->SetMat4f("proj", proj);
        modelShader->SetMat4f("view", view);

        outlineShader->Bind();
        outlineShader->SetMat4f("proj", proj);
        outlineShader->SetMat4f("view", view);

        transparentShader->Bind();
        transparentShader->SetMat4f("proj", proj);
        transparentShader->SetMat4f("view", view);
        transparentShader->SetDirectionalLight(dirLight);
        transparentShader->SetSpotLight(spotLight);

        for (int i = 0; i < sizeof(positions) / sizeof(glm::vec3); i++) {
            bag->SetPos(positions[i]);
            bag->Draw(modelShader, outlineShader);
        }

        // 绘制透明物体
        GLCall(glEnable(GL_BLEND));
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i =0; i < sizeof(windowsPositions)/ sizeof(glm::vec3); i++) {
            float length = glm::distance(windowsPositions[i], camera.GetPos());
            renderOrder[length] = windowsPositions[i];
        }

        for (auto it = renderOrder.rbegin(); it != renderOrder.rend(); ++it) {
            window->SetPos(it->second);
            window->Draw(transparentShader, outlineShader);
        }
        renderOrder.clear();
        GLCall(glDisable(GL_BLEND));

        modelShader->ResetLightIdx(); // 重设光照的索引

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


