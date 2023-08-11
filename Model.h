#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "Shader.h"

struct TextureIndex;
class ShaderPtr;
//class ShaderPtr;
// struct aiScene;
// struct aiNode;
// struct aiMesh;
// struct aiMaterial;

class Model{
public:
    static std::unique_ptr<Model> CreateModel(std::string path);
    static std::unique_ptr<Model> CreateModel(std::vector<MeshPtr>& meshs);
    static std::unique_ptr<Model> CreateModel(std::vector<MeshPtr>&& meshs);

    void Draw(ShaderPtr& shader);
    void DrawInstancing(ShaderPtr& shader, unsigned int count);
    void SetPos(float x, float y, float z);
    void SetPos(glm::vec3& pos);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);
    void SetOutline(bool isOn){_isDrawOutline = isOn;}
    void SetVertexLayout(const VertexBufferLayout& layout, VertexBuffer* buffer);
    int LoadTextureFromPath(const std::string& path, unsigned int type, unsigned int format);
    void PrintLoadedTextures();
    ~Model(){}

private:
    Model();
    void setupVertexLayout();
    void setupModel(std::vector<MeshPtr>& meshs);
    void setupModel(std::vector<MeshPtr>&& meshs);
    void setupModel(std::string name);
    void processNode(const aiNode* node, const aiScene* scene);
    void loadMesh(const aiMesh* mesh, const aiScene* scene);
    std::vector<TextureIndex> loadTexture(const aiMaterial* material, aiTextureType textureType, TextureUsage usage);

    VertexBufferLayout _layout;
    ShaderPtr _outlineShader;
    std::vector<MeshPtr> _meshs;
    std::string _directory;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    bool _isDrawOutline{false};
};
