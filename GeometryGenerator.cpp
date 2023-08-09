#include <cmath>
#include <cassert>
#include "GeometryGenerator.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

MeshPtr GeometryGenerator::generateCube()
{
    std::vector<Vertex> verts;

    // 前面
    verts.push_back({-0.5, 0.5, 0.5, 0.0f, 0.0f, 1.0f,     0.0f, 1.0f});
    verts.push_back({0.5, 0.5, 0.5, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f});
    verts.push_back({0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f,     1.0f, 0.0f});
    verts.push_back({-0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f});
    //背面
    verts.push_back({-0.5, 0.5, -0.5, 0.0f, 0.0f, -1.0f,0.0f, 1.0f});
    verts.push_back({0.5, 0.5, -0.5,  0.0f, 0.0f, -1.0f,   1.0f, 1.0f});
    verts.push_back({0.5, -0.5, -0.5,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f});
    verts.push_back({-0.5, -0.5, -0.5,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f});

    // 左面
    verts.push_back({-0.5, 0.5, -0.5, -1.0f, 0.0f, 0.0f,   1.0f, 1.0f});
    verts.push_back({-0.5, 0.5, 0.5, -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,});
    verts.push_back({-0.5, -0.5, 0.5, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f});
    verts.push_back({-0.5, -0.5, -0.5, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f});

    // 右面
    verts.push_back({0.5, 0.5, -0.5, 1.0f, 0.0f, 0.0f,     1.0f, 1.0f});
    verts.push_back({0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f,      1.0f, 0.0f});
    verts.push_back({0.5, -0.5, 0.5, 1.0f, 0.0f, 0.0f,     0.0f, 0.0f});
    verts.push_back({0.5, -0.5, -0.5, 1.0f, 0.0f, 0.0f,    0.0f, 1.0f});

    // 上面
    verts.push_back({-0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f,   0.0f, 1.0f});
    verts.push_back({-0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f});
    verts.push_back({0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f});
    verts.push_back({0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f,    1.0f, 1.0f});

    // 下面
    verts.push_back({-0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f});
    verts.push_back({-0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f});
    verts.push_back({0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,   1.0f, 0.0f,});
    verts.push_back({0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f,  1.0f, 1.0f,});

    std::vector<unsigned int> indices = {
        //,前面
        0,2,1,
        2,0,3,

        //,后面
        5, 6, 7, 
        5, 7, 4,

        //,左面
        8,11,10,
        8, 10, 9,

        //,右面
        12,14,15,
        14,12,13,

        //,上面
        16,18,19,
        18,16,17,

        //,下面
        20,23,21,
        21,23,22,
    };

    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    std::vector<TextureIndex> texIdxs;

    return MeshManager::CreateMesh(std::move(verts), std::move(indices), std::move(texIdxs), layout);
}

MeshPtr GeometryGenerator::generatePlane()
{
    std::vector<Vertex> verts;
    std::vector<TextureIndex> texs;
    std::vector<unsigned int> indices = {
        0, 2, 3,
        0, 3, 1
    };
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    verts.push_back({-1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f});
    verts.push_back({ 1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f});
    verts.push_back({-1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f});
    verts.push_back({ 1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f});

    return MeshManager::CreateMesh(std::move(verts), std::move(indices), std::move(texs), layout);
}

MeshPtr GeometryGenerator::generateQuad()
{
    std::vector<Vertex> verts;
    std::vector<TextureIndex> textures;
    VertexBufferLayout layout;

    Vertex v0, v1, v2, v3;

    v0.pos = glm::vec3(-1.0f, 1.0f, 0.0f);
    v0.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v0.tex = glm::vec2(0.0f, 1.0f);

    v1.pos = glm::vec3(1.0f, 1.0f, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.tex = glm::vec2(1.0f, 1.0f);

    v2.pos = glm::vec3(1.0f, -1.0f, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.tex = glm::vec2(1.0f, 0.0f);

    v3.pos = glm::vec3(-1.0f, -1.0f, 0.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.tex = glm::vec2(0.0f, 0.0f);

    verts.push_back(v0); verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);

    std::vector<unsigned int> indices = {
        0, 3, 2,
        0, 2, 1,
    };

    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    return MeshManager::CreateMesh(std::move(verts), std::move(indices), std::move(textures), layout);
}

MeshPtr GeometryGenerator::generateSkyBox()
{
    std::vector<Vertex> verts;
    std::vector<TextureIndex> textures;

    verts.push_back({-1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({ 1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({-1.0f, -1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({ 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});

    verts.push_back({-1.0f, 1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({ 1.0f, 1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({-1.0f, -1.0f,-1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});
    verts.push_back({ 1.0f, -1.0f,-1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f});


    std::vector<unsigned int> indices = {

        // 前面
        0, 3, 2,
        0, 1, 3,

        // 背面
        4, 6, 7,
        4, 7, 5,

        // 顶面
        4, 1, 0,
        4, 5, 1,

        // 底面
        6, 2, 3,
        6, 3, 7,

        // 左面
        0, 2, 6, 
        0, 6, 4,

        // 右面
        5, 7, 3,
        5, 3, 1,
    };

    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);


    MeshPtr ptr = MeshManager::CreateMesh(verts, indices, textures, layout);
    return ptr;
}
