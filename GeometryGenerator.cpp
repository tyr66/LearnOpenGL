#include <cmath>
#include <cassert>
#include "GeometryGenerator.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

void GeometryGenerator::generateCube(std::vector<float>& verts, std::vector<unsigned int>& indices, float w, float h , float d)
{
    verts.clear();
    verts = {
        // 背面 
        -0.5, 0.5, -0.5, 0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
        0.5, 0.5, -0.5,  0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
        0.5, -0.5, -0.5,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
        -0.5, -0.5, -0.5,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        // 前面 
        -0.5, 0.5, 0.5, 0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.5, 0.5, 0.5, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,
        0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        -0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

        // 左面
        -0.5, 0.5, -0.5, -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -0.5, 0.5, 0.5, -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        -0.5, -0.5, 0.5, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5, -0.5, -0.5, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

        // 右面
        0.5, 0.5, -0.5, 1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
        0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f,      1.0f, 0.0f,
        0.5, -0.5, 0.5, 1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        0.5, -0.5, -0.5, 1.0f, 0.0f, 0.0f,    0.0f, 1.0f,

        // 上面
        -0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

        // 下面
        -0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
    };

    for (int i = 0 ; i< verts.size(); i+=3)
    {
        verts[i] *= w;
        verts[i + 1] *= h;
        verts[i + 2] *= d;
    }

    indices.clear();
    indices = {
        //,前面
        0,2,1,
        2,0,3,

        //,后面
        4,6,7,
        6,4,5,

        //,左面
        8,10,11,
        10,8,9,

        //,右面
        12,14,15,
        14,12,13,

        //,上面
        16,18,19,
        18,16,17,

        //,下面
        20,22,23,
        22,20,21,
    };
}
void GeometryGenerator::generateSphere(std::vector<float>& verts, std::vector<unsigned int >&indices, float r)
{

}
std::unique_ptr<Model> GeometryGenerator::generateWindowQuad()
{
    std::vector<Vertex> verts;
    std::vector<TextureIndex> textures;
    std::vector<MeshPtr> meshs;
    VertexBufferLayout layout;

    Vertex v0, v1, v2, v3;

    v0.pos = glm::vec3(-1.0f, 1.0f, 0.0f);
    v0.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v0.tex = glm::vec2(0.0f, 0.0f);

    v1.pos = glm::vec3(1.0f, 1.0f, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.tex = glm::vec2(1.0f, 0.0f);

    v2.pos = glm::vec3(1.0f, -1.0f, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.tex = glm::vec2(1.0f, 1.0f);

    v3.pos = glm::vec3(-1.0f, -1.0f, 0.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.tex = glm::vec2(0.0f, 1.0f);

    verts.push_back(v0); verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);

    std::vector<unsigned int> indices = {
        0, 3, 1,
        2, 1, 3,
    };

    TextureIndex texIdx;
    texIdx.idx = TextureGenerator::LoadTexture("./textures/blending_transparent_window.png", GL_TEXTURE_2D);
    texIdx.usage = TextureUsage::TEXTURE_USAGE_DIFFUSE;
    textures.push_back(texIdx);
    
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    auto mesh = MeshManager::CreateMesh(std::move(verts), std::move(indices), std::move(textures), layout);
    meshs.push_back(mesh);

    return Model::CreateModel(meshs);
}

