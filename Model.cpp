#include <iostream>
#include <stdexcept>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include "help.h"

std::unique_ptr<Model> Model::CreateModel(std::string path)
{
    auto model = std::unique_ptr<Model>(new Model());
    model->setupModel(path);
    return model;
}

void Model::Draw(ShaderPtr& shader, ShaderPtr& outlineShader)
{

    if (_isDrawOutline) {

        shader->Bind();
        GLCall(glEnable(GL_STENCIL_TEST));
        GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GLCall(glStencilMask(0xFF));
        GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
        for (unsigned int i = 0; i < _meshs.size(); i++) {
            _meshs[i].Draw(shader, _textures_loaded);
        }

        outlineShader->Bind();
        GLCall(glDisable(GL_DEPTH_TEST))
        GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
        GLCall(glStencilMask(0x00));
        outlineShader->SetFloat("outlineScale", 0.01f);

        for (unsigned int i = 0; i < _meshs.size(); i++) {
            _meshs[i].Draw(outlineShader, _textures_loaded);
        }
        
        GLCall(glStencilMask(0xFF));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_STENCIL_TEST));

    } else {

        shader->Bind();
        for (unsigned int i = 0; i < _meshs.size(); i++)
        {
            _meshs[i].Draw(shader, _textures_loaded);
        }
    }

}

void Model::SetPos(float x, float y, float z)
{
    _translate += glm::vec3(x, y, z);
    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, glm::vec3(x, y, z));
    _modelMat = translate * _modelMat;
}

void Model::SetRotation(float x, float y, float z)
{
    _rotation += glm::vec3(x, y, z);
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
    _modelMat = rotation * _modelMat;
}

void Model::SetScale(float x, float y, float z)
{
    _scale *= glm::vec3(x, y, z);
    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(scale, glm::vec3(x, y, z));
    _modelMat = scale * _modelMat;
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

    auto i = material->GetTextureCount(aiTextureType::aiTextureType_AMBIENT);

    textures.insert(textures.end(), diffuseTexs.begin(), diffuseTexs.end());
    textures.insert(textures.end(), specularTexs.begin(), specularTexs.end());

    Mesh mesh;
    mesh.SetupMesh(std::move(vertexs), std::move(indices), std::move(textures), _layout);

    _meshs.push_back(std::move(mesh));
}

std::vector<TextureIndex> Model::loadTexture(const aiMaterial* material, aiTextureType textureType, TextureUsage usage)
{
    std::vector<TextureIndex> res;

    for (unsigned int i = 0; i < material->GetTextureCount(textureType); i++)
    {
        TextureIndex texIndex;
        aiString texName;
        std::string name;
        bool isGenerated = false;

        material->GetTexture(textureType, i, &texName);
        name = texName.C_Str();

        for (unsigned int j = 0; j < _textures_loaded.size(); j++) {

            if (name == _textures_loaded[j]->GetName())
            {
                // 该贴图已经被加载
                texIndex.usage = usage;
                texIndex.idx = j;
                isGenerated = true;
                std::cout << "find a loaded texture" << std::endl;
                break;
            }
        }

        if (!isGenerated) {
            // 该贴图还没有创建
            std::string path = _directory + '/'+ texName.C_Str();
            auto tex = Texture::CreateTexture(path, name,  GL_TEXTURE_2D, GL_RGB);

            std::cout << "load a texture name: " << tex->GetName() << std::endl;

            _textures_loaded.push_back(std::move(tex));
            texIndex.idx = _textures_loaded.size() - 1;
            texIndex.usage = usage;

        }

        res.push_back(texIndex);
    }

    return res;
}

void Model::PrintLoadedTextures()
{
    for (int i = 0; i < _textures_loaded.size(); i++)
    {
         std::cout << "loaded texture name: " << _textures_loaded[i]->GetName() << std::endl;
    }
}

glm::mat4 Model::GetModelMat()
{
    return _modelMat;
}

