#include <stdexcept>
#include <cassert>
#include <stb_image.h>

#include "Texture.h"
#include "glad/glad.h"
#include "help.h"

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;
bool TextureManager::isInit = false;

bool TextureManager::IsLoaded(const std::string& fullName)
{
    return instance->_generatedMap.count(fullName) > 0;
}

void TextureManager::Init()
{
    // 初始化并加载默认的贴图
    if (!isInit) {
        instance.reset(new TextureManager());
        isInit = true;

    }
}

void TextureManager::Clear()
{
    instance->_generatedMap.clear();
    instance->_textures.clear();
    instance->id = 0;
}

Texture* TextureManager::GetTexture(int id)
{
    if (instance->_textures.count(id) > 0) {
        return &(*instance->_textures[id].get());
    }

    return nullptr;
}

int TextureManager::GetTextureIndex(const std::string& fullName)
{
    if (instance->_generatedMap.count(fullName) > 0)
        return instance->_generatedMap[fullName];

    return -1;
}

Texture* TextureManager::GetTexture(const std::string& fullName)
{
    if (instance->_generatedMap.count(fullName) > 0)
    {
        return GetTexture(instance->_generatedMap[fullName]);
    }
    return nullptr;
}

int TextureManager::LoadTexture(const std::string& fullName, unsigned int type)
{
    if (instance->_generatedMap.count(fullName) > 0) {
        std::cout << "[INFO] already loaded texture : " << fullName << std::endl;
        return instance->_generatedMap[fullName];
    }

    unsigned int texture = 0;
    unsigned char* data = nullptr;
    int height, width, nrChannels;
    unsigned int format = 0;

    data = stbi_load(fullName.c_str(), &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        std::cout << "[Warning] failed to load texture :" << fullName << std::endl;
        throw std::runtime_error("");
    }

    try{
        GLCall(glGenTextures(1, &texture));
        glBindTexture(type, texture);

        if (nrChannels == 3) {
            format = GL_RGB;
        }else if (nrChannels == 4) {
            format = GL_RGBA;
        } else if (nrChannels == 1){
            format = GL_RED;
        } else{
            std::cout << "[ERROR] texture"<< fullName << "unknow channels: " << nrChannels << std::endl;
            assert(false);
        }

        switch(type)
        {
            case GL_TEXTURE_2D: 
                GLCall(glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
                break;
            case GL_TEXTURE_1D:
                assert(false);
                break;
        }
    }catch(std::exception& e)
    {
        stbi_image_free(data);
        throw e;
    }

    stbi_image_free(data);
    GLCall(glGenerateMipmap(type));

    auto ptr = std::unique_ptr<Texture>(new Texture(texture, fullName, type, format));

    std::cout << "[INFO] loaded texture : " << fullName << std::endl;

    instance->_generatedMap[fullName] = ++instance->id;
    instance->_textures[instance->id] = std::move(ptr);
    return instance->id;
}

int TextureManager::LoadCubeMap(const std::string& name, const std::vector<std::string>& facePath)
{
    stbi_set_flip_vertically_on_load(false);
    if (instance->_generatedMap.count(name) > 0) {
        std::cout << "[INFO] already loaded cube map : " << name << std::endl;
        return instance->_generatedMap[name];
    }

    unsigned int texture = 0;
    unsigned char* data = nullptr;
    int height, width, nrChannels;
    unsigned int format = 0;

    GLCall(glGenTextures(1, &texture));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));

    int i = 0;
    for (const auto& path : facePath)
    {
        data = stbi_load(path.c_str(), &width ,&height, &nrChannels, 0);

        if (!data) {
            GLCall(glDeleteTextures(1, &texture));
            std::cout << "[ERROR] failed to load cube map face : " << path <<std::endl;
            throw std::runtime_error("");
        }

        switch (nrChannels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: assert(false);
        }

        GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        stbi_image_free(data);
        i++;
    }

    stbi_set_flip_vertically_on_load(true);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    instance->_generatedMap[name] = ++instance->id;
    instance->_textures[instance->id] = std::unique_ptr<Texture>(new Texture(texture, name, GL_TEXTURE_CUBE_MAP, format));
    return instance->id;
}


int TextureManager::GetTextureRenderID(int id)
{
    return instance->_textures[id]->_renderID;
}

int TextureManager::CreateTexture(const std::string& texName, unsigned int type, unsigned int format, unsigned int w, unsigned int h, void* data)
{
    if (instance->_generatedMap.count(texName) > 0) {
        std::cout << "[INFO] texture : " << texName << "already create" << std::endl; 
        return instance->_generatedMap[texName];
    }

    unsigned int texID;
    GLCall(glGenTextures(1, &texID));
    GLCall(glBindTexture(type, texID));
        
    switch(type)
    {
        case GL_TEXTURE_2D:
            GLCall(glTexImage2D(type, 0 ,format, w, h, 0, format, GL_UNSIGNED_BYTE, data));
            GLCall(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        break;

        case GL_TEXTURE_2D_MULTISAMPLE:
            GLCall( glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, w, h, GL_TRUE));
        break;
        default:
            assert(false);
        break;
    }


    auto ptr = std::unique_ptr<Texture>(new Texture(texID, texName, type, format));

    instance->_generatedMap[texName] = ++instance->id;
    instance->_textures[instance->id] = std::move(ptr);

    std::cout << "[INFO] create a new texture : " << texName << std::endl;

    return instance->id;
}

Texture::Texture(unsigned int id, std::string name, unsigned int type, unsigned int format): _type(type), _format(format),_renderID(id),  _name(name)
{

}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &_renderID));
    std::cout << "[INFO] " << "delete texture :" << _name << std::endl;
}


void Texture::SetFiltering(unsigned int minFilter, unsigned int magFilter)
{
    Bind();
    GLCall(glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, minFilter));
    GLCall(glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, magFilter));
}

void Texture::SetWrapping(unsigned int wrap_s, unsigned int wrap_t)
{
    Bind();
    GLCall(glTexParameteri(_type, GL_TEXTURE_WRAP_S, wrap_s));
    GLCall(glTexParameteri(_type, GL_TEXTURE_WRAP_T, wrap_t));
}

void Texture::BindAndActive(unsigned int unit)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + unit));
    Bind();
}

void Texture::Bind()
{
    GLCall(glBindTexture(_type, _renderID));
}

void Texture::Unbind()
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
