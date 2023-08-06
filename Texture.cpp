#include <stdexcept>
#include <cassert>
#include <stb_image.h>

#include "Texture.h"
#include "glad/glad.h"
#include "help.h"

std::unique_ptr<TextureGenerator> TextureGenerator::instance = nullptr;
bool TextureGenerator::isInit = false;

bool TextureGenerator::IsLoaded(const std::string& fullName)
{
    return instance->_generatedMap.count(fullName) > 0;
}

void TextureGenerator::Init()
{
    // 初始化并加载默认的贴图
    if (!isInit) {
        instance.reset(new TextureGenerator());
        isInit = true;

    }
}

void TextureGenerator::Clear()
{
    instance->_generatedMap.clear();
    instance->_textures.clear();
    instance->id = 0;
}

Texture* TextureGenerator::GetTexture(int id)
{
    if (instance->_textures.count(id) > 0) {
        return &(*instance->_textures[id].get());
    }

    return nullptr;
}

int TextureGenerator::GetTextureIndex(const std::string& fullName)
{
    if (instance->_generatedMap.count(fullName) > 0)
        return instance->_generatedMap[fullName];

    return -1;
}

Texture* TextureGenerator::GetTexture(const std::string& fullName)
{
    if (instance->_generatedMap.count(fullName) > 0)
    {
        return GetTexture(instance->_generatedMap[fullName]);
    }
    return nullptr;
}

int TextureGenerator::LoadTexture(const std::string& fullName, unsigned int type)
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
        } else {
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

Texture::Texture(unsigned int id, std::string name, unsigned int type, unsigned int format): _renderID(id), _type(type), _format(format), _name(name)
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
    GLCall(glActiveTexture(unit));
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
