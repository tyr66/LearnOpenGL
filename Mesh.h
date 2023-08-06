#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>

#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "VertexArray.h"
#include "Texture.h"

class Texture;
class ShaderPtr;

enum TextureUsage
{
    TEXTURE_USAGE_DIFFUSE = 0,
    TEXTURE_USAGE_SPECULAR = 1,
};

struct Vertex{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex;

};

struct TextureIndex
{
    int idx;
    TextureUsage usage;

};

class Mesh {
public:
    void Draw(ShaderPtr& shader);

    void SetTexture(TextureIndex texIndex);

    std::vector<Vertex> vertexs;
    std::vector<unsigned int> indices;
    std::vector<TextureIndex> textures;

    friend class MeshManager;

private:
     void SetupMesh(std::vector<Vertex>&verts, std::vector<unsigned int>&indics,std::vector<TextureIndex>&texs, VertexBufferLayout& layout);
     void SetupMesh(std::vector<Vertex>&&verts, std::vector<unsigned int>&&indics,std::vector<TextureIndex>&&texs, VertexBufferLayout& layout);

private:
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndiceBuffer> _ebo;
    std::unique_ptr<VertexArray> _vao;
};

class MeshPtr {
public:
    MeshPtr(Mesh* _mesh, int id);
    Mesh* operator->();
private:
    Mesh* _mesh;
    int _id;
};

class MeshManager {
public:
    static void Init();
    static void Clear();
	static MeshPtr CreateMesh(std::vector<Vertex>& verts, std::vector<unsigned int>& indices, std::vector<TextureIndex>& textures, VertexBufferLayout& layout);
    static MeshPtr CreateMesh(std::vector<Vertex>&& verts, std::vector<unsigned int>&& indices, std::vector<TextureIndex>&& textures,VertexBufferLayout& layout); 
private:
    MeshManager();
    static std::unique_ptr<MeshManager> instance;
    static bool isInit;
    static int id;

private:
    std::map<std::string, int> _nameToIdx;
    std::unordered_map<unsigned int, std::unique_ptr<Mesh>> _meshs;
    
};
