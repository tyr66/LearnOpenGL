#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"

struct TextureIndex;
class ShaderPtr;
// struct aiScene;
// struct aiNode;
// struct aiMesh;
// struct aiMaterial;

class Model{
public:
    static std::unique_ptr<Model> CreateModel(std::string path);
    void Draw(ShaderPtr& shader, ShaderPtr& outlineShader);
    void SetPos(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);
    void SetOutline(bool isOn){_isDrawOutline = isOn;}
    void PrintLoadedTextures();
    glm::mat4 GetModelMat();
    ~Model(){}

private:
    Model():_translate(glm::vec3(0.0f)), _scale(glm::vec3(1.0f)), _rotation(glm::vec3(0.0f)), _modelMat(glm::mat4(1.0f)){};
    void setupVertexLayout();
    void setupModel(std::string name);
    void processNode(const aiNode* node, const aiScene* scene);
    void loadMesh(const aiMesh* mesh, const aiScene* scene);
    std::vector<TextureIndex> loadTexture(const aiMaterial* material, aiTextureType textureType, TextureUsage usage);

    std::vector<std::unique_ptr<Texture>> _textures_loaded;
    VertexBufferLayout _layout;
    std::vector<Mesh> _meshs;
    std::string _directory;
    glm::vec3 _translate;
    glm::vec3 _rotation;
    glm::vec3 _scale;
    glm::mat4 _modelMat;

    bool _isDrawOutline{false};
};
