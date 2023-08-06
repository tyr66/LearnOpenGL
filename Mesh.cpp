#include "Mesh.h"
#include "help.h"
#include "Shader.h"

bool MeshManager::isInit = false;
std::unique_ptr<MeshManager> MeshManager::instance;
int MeshManager::id = 0;

void MeshManager::Init()
{
    if (!isInit) {
        instance.reset(new MeshManager());
        isInit = true;
    }
}

void MeshManager::Clear()
{
    instance->_meshs.clear();
    instance->_nameToIdx.clear();
}

MeshPtr MeshManager::CreateMesh(std::vector<Vertex>&verts, std::vector<unsigned int>&indices, std::vector<TextureIndex>& textures, VertexBufferLayout& layout)
{
    Mesh* mesh = new Mesh();
    mesh->SetupMesh(verts, indices, textures, layout);
    instance->_meshs[++id] = std::unique_ptr<Mesh>(mesh);
    return MeshPtr(mesh, id);
}


MeshPtr MeshManager::CreateMesh(std::vector<Vertex>&& verts, std::vector<unsigned int>&& indices, std::vector<TextureIndex>&& textures,VertexBufferLayout& layout)
{
    Mesh* mesh = new Mesh();
    mesh->SetupMesh(std::move(verts), std::move(indices), std::move(textures), layout);
    instance->_meshs[++id] = std::unique_ptr<Mesh>(mesh);
    return MeshPtr(mesh, id);
}

MeshManager::MeshManager()
{
}

MeshPtr::MeshPtr(Mesh* mesh, int id): _mesh(mesh), _id(id)
{
}


Mesh* MeshPtr::operator->()
{
    return _mesh;
}


void Mesh::Draw(ShaderPtr& shader)
{

    //TODO 设置贴图
    unsigned int diffuse_id = 0;
    unsigned int specular_id = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + i));
		std:: string name = "material.";
        auto& texIdx = textures[i];
        auto tex = TextureGenerator::GetTexture(texIdx.idx);

        switch (texIdx.usage)
        {
        case TextureUsage::TEXTURE_USAGE_DIFFUSE:
            name += Shader::DiffuseTexturePrefix + std::to_string(diffuse_id++);
            break;
        case TextureUsage::TEXTURE_USAGE_SPECULAR:
            name += Shader::SpecularTexturePrefix + std::to_string(specular_id++);
            break;
        }
		shader->SetInt(name, i);
		tex->Bind();
    }

    if (diffuse_id == 0) {
        //std::cout << "[WARNING] mesh dont have diffuse texture, so just default diffuse color" << std::endl;
        shader->SetInt("material.enableDiffuse", 0);
    }else {
        shader->SetInt("material.enableDiffuse", 1);

    }

    if (specular_id == 0) {
        // 采用默认的高光贴图
        //std::cout << "[INFO] mesh dont have specular texture, so just disable specular" << std::endl;
        shader->SetInt("material.enableSpecular", 0);
    } else {
        shader->SetInt("material.enableSpecular", 1);
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

void Mesh::SetTexture(TextureIndex texIndex)
{
    for (auto& tex : textures)
    {
        if (tex.idx == texIndex.idx && tex.usage == texIndex.usage)
            return;
        if (tex.usage == texIndex.usage) {
            tex = texIndex;
            return;
        }
    }

    textures.push_back(texIndex);
}

