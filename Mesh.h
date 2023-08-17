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
    TEXTURE_USAGE_SKYBOX = 2,
    TEXTURE_USAGE_FRAMEBUFFER = 3,
    TEXTURE_USAGE_SHADOWMAP = 4,
    TEXTURE_USAGE_SHADOWCUBEMAP = 5,
};

struct Vertex{

    Vertex(){}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty){
        pos = glm::vec3(px, py, pz);
        normal = glm::vec3(nx, ny, nz);
        tex = glm::vec2(tx, ty);
    }

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
    void DrawInstancing(ShaderPtr& shader, unsigned int count);

    void SetTexture(TextureIndex texIndex);
    void SetVertexLayout(const VertexBufferLayout& layout, VertexBuffer* buffer);
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
