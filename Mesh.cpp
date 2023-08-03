#include "Mesh.h"
#include "help.h"
#include "Shader.h"


void Mesh::Draw(ShaderPtr& shader, std::vector<std::unique_ptr<Texture>>& texBuffer)
{

    //TODO 设置贴图
    unsigned int diffuse_id = 0;
    unsigned int specular_id = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + i));
		std:: string name = "";
        auto& texIdx = textures[i];

        switch (texIdx.usage)
        {
        case TextureUsage::TEXTURE_USAGE_DIFFUSE:
            name = Shader::DiffuseTexturePrefix + std::to_string(++diffuse_id);
            break;
        case TextureUsage::TEXTURE_USAGE_SPECULAR:
            name = Shader::SpecularTexturePrefix + std::to_string(++specular_id);
            break;
        }
		shader->SetInt(name, i);
        texBuffer[texIdx.idx]->Bind();
    }


    _vao->Bind();
    _ebo->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0));
    _ebo->Unbind();
    _vao->Unbind();
}

void Mesh::SetupMesh(std::vector<Vertex>&verts, std::vector<unsigned int>&indics,std::vector<TextureIndex>&texs, VertexBufferLayout& layout)
{
    vertexs = verts; indices =  indics; textures = texs;

    _vbo = VertexBuffer::CreateVertexBuffer(vertexs.data(), vertexs.size() * sizeof(Vertex));
    _ebo = IndiceBuffer::CreateIndiceBuffer(indices.data(), indices.size());
    _vao = VertexArray::CreateVertexArray();

    _vao->Bind();
    _vbo->Bind();
    _ebo->Bind();

    _vao->AddBuffer(*_vbo.get(), layout);

    _ebo->Unbind();
    _vbo->Unbind();
    _vao->Unbind();

}

void Mesh::SetupMesh(std::vector<Vertex>&&verts, std::vector<unsigned int>&&indics,std::vector<TextureIndex>&&texs, VertexBufferLayout& layout)
{
    vertexs = std::move(verts); indices = std::move(indics); textures = std::move(texs);
    _vbo = VertexBuffer::CreateVertexBuffer(vertexs.data(), vertexs.size() * sizeof(Vertex));
    _ebo = IndiceBuffer::CreateIndiceBuffer(indices.data(), indices.size());
    _vao = VertexArray::CreateVertexArray();

    _vao->Bind();
    _vbo->Bind();
    _ebo->Bind();

    _vao->AddBuffer(*_vbo.get(), layout);

    _ebo->Unbind();
    _vbo->Unbind();
    _vao->Unbind();
}

