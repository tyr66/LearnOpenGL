#include <iostream>
#include <stdexcept>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "help.h"
#include "Mesh.h"

Model::Model():_position(glm::vec3(0.0f)),_rotation(glm::vec3(0.0f)),_scale(glm::vec3(1.0f)){

}
std::unique_ptr<Model> Model::CreateModel(std::string path)
{
    auto model = std::unique_ptr<Model>(new Model());
    model->setupModel(path);
    return model;
}

std::unique_ptr<Model> Model::CreateModel(std::vector<MeshPtr>& meshs)
{
    auto res = std::unique_ptr<Model>(new Model());
    res->setupModel(meshs);
    return res;
}
std::unique_ptr<Model> Model::CreateModel(std::vector<MeshPtr>&& meshs)
{
    auto res = std::unique_ptr<Model>(new Model());
    res->setupModel(std::move(meshs));
    return res;
}

void Model::Draw(ShaderPtr& shader, ShaderPtr& outlineShader)
{

    // 计算出model 矩阵, 因为glm使用矩阵右乘所以需要调整矩阵的乘法顺序
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, _position);
    model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, _scale);

    if (_isDrawOutline) {

        shader->Bind();
        shader->SetMat4f("model", model);

        GLCall(glEnable(GL_STENCIL_TEST));
        GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCall(glStencilMask(0xFF));
        GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
        for (unsigned int i = 0; i < _meshs.size(); i++) {
            _meshs[i]->Draw(shader);
        }

        outlineShader->Bind();
        outlineShader->SetMat4f("model", model);
        GLCall(glDisable(GL_DEPTH_TEST))
        GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
        GLCall(glStencilMask(0x00));
        outlineShader->SetFloat("outlineScale", 0.01f);

        for (unsigned int i = 0; i < _meshs.size(); i++) {
            _meshs[i]->Draw(outlineShader);
        }
        
        GLCall(glStencilMask(0xFF));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_STENCIL_TEST));

    } else {

        shader->Bind();
        shader->SetMat4f("model", model);
        for (unsigned int i = 0; i < _meshs.size(); i++)
        {
            _meshs[i]->Draw(shader);
        }
    }

}

void Model::SetPos(float x, float y, float z)
{
    _position.x = x;
    _position.y = y;
    _position.z = z;
}

void Model::SetPos(glm::vec3& pos)
{
    _position = pos;
}

void Model::SetRotation(float x, float y, float z)
{
    _rotation.x = x;
    _rotation.y = y;
    _rotation.z = z;
}

void Model::SetScale(float x, float y, float z)
{
    _scale.x = x;
    _scale.y = y;
    _scale.z = z;
}

void Model::setupVertexLayout()
{
    _layout.push<float>(3); // pos
    _layout.push<float>(3); // normal
    _layout.push<float>(2); // texcoord
}

void Model::setupModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* sceneobj = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (sceneobj == nullptr || sceneobj->mFlags & AI_SCENE_FLAGS_INCOMPLETE || sceneobj->mRootNode == nullptr) {
        std::cout << "[ERROR] : failed to load model from " << path << std::endl;
        throw std::runtime_error("load file error");
    }

    _directory = path.substr(0, path.find_last_of('/'));

    setupVertexLayout();
    processNode(sceneobj->mRootNode, sceneobj);
}

void Model::setupModel(std::vector<MeshPtr>& meshs)
{
     _meshs = meshs;
    _directory = "create by script";
}
void Model::setupModel(std::vector<MeshPtr>&& meshs)
{
    _meshs = std::move(meshs);
    _directory = "create by script";
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        loadMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(const aiMesh* aimesh, const aiScene *scene)
{
    assert(aimesh != nullptr);

    std::vector<Vertex> vertexs;
    std::vector<unsigned int> indices;
    std::vector<TextureIndex> textures;

    for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.pos.x = aimesh->mVertices[i].x;
        vertex.pos.y = aimesh->mVertices[i].y;
        vertex.pos.z = aimesh->mVertices[i].z;

        if (aimesh->HasNormals()) {
            vertex.normal.x = aimesh->mNormals[i].x;
            vertex.normal.y = aimesh->mNormals[i].y;
            vertex.normal.z = aimesh->mNormals[i].z;
        }

        if (aimesh->mTextureCoords[0]) {
            vertex.tex.x = aimesh->mTextureCoords[0][i].x;
            vertex.tex.y = aimesh->mTextureCoords[0][i].y;
        } else 
            vertex.tex = glm::vec2(0.0f);

        vertexs.push_back(vertex);
    }

    for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
    {
        aiFace face = aimesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    const aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];

    auto diffuseTexs = loadTexture(material, aiTextureType::aiTextureType_DIFFUSE, TextureUsage::TEXTURE_USAGE_DIFFUSE);
    auto specularTexs = loadTexture(material, aiTextureType::aiTextureType_SPECULAR, TextureUsage::TEXTURE_USAGE_SPECULAR);

    textures.insert(textures.end(), diffuseTexs.begin(), diffuseTexs.end());
    textures.insert(textures.end(), specularTexs.begin(), specularTexs.end());

    MeshPtr mesh = MeshManager::CreateMesh(std::move(vertexs), std::move(indices), std::move(textures), _layout);
    _meshs.push_back(std::move(mesh));
}

std::vector<TextureIndex> Model::loadTexture(const aiMaterial* material, aiTextureType textureType, TextureUsage usage)
{
    std::vector<TextureIndex> res;

    for (unsigned int i = 0; i < material->GetTextureCount(textureType); i++)
    {
        TextureIndex texIndex;
        aiString texName;
        std::string fullName;

        material->GetTexture(textureType, i, &texName);
        fullName = _directory + '/' + texName.C_Str();
        texIndex.idx = TextureManager::LoadTexture(fullName, GL_TEXTURE_2D);
        texIndex.usage = usage;
        res.push_back(texIndex);
    }

    return res;
}
