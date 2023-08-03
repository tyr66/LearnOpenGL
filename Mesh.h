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
    void Draw(ShaderPtr& shader, std::vector<std::unique_ptr<Texture>>& texBuffer);
    void SetupMesh(std::vector<Vertex>&verts, std::vector<unsigned int>&indics,std::vector<TextureIndex>&texs, VertexBufferLayout& layout);
    void SetupMesh(std::vector<Vertex>&&verts, std::vector<unsigned int>&&indics,std::vector<TextureIndex>&&texs, VertexBufferLayout& layout);

    std::vector<Vertex> vertexs;
    std::vector<unsigned int> indices;
    std::vector<TextureIndex> textures;

private:
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndiceBuffer> _ebo;
    std::unique_ptr<VertexArray> _vao;

};
