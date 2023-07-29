#include <stdexcept>
#include <cassert>
#include <stb_image.h>

#include "Texture.h"
#include "glad/glad.h"
#include "help.h"

Texture::Texture(unsigned int id, std::string name, unsigned int type, unsigned int format): _renderID(id), _type(type), _format(format), _name(name)
{

}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &_renderID));
}

std::unique_ptr<Texture> Texture::CreateTexture(std::string path, std::string name, unsigned int type, unsigned int format)
{
    unsigned int texture = 0;
    unsigned char* data = nullptr;
    int height, width, nrChannels;
    unsigned int fileFormat;

    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        std::cout << "failed to load texture :" << path << std::endl;
        throw std::runtime_error("");
    }

    try{
        GLCall(glGenTextures(1, &texture));
        glBindTexture(type, texture);

        if (nrChannels == 3) {
            fileFormat = GL_RGB;
        }else if (nrChannels == 4) {
            fileFormat = GL_RGBA;
        } else {
            std::cout << "unknow channels: " << nrChannels << std::endl;
            assert(false);
        }

        switch(type)
        {
            case GL_TEXTURE_2D: 
                GLCall(glTexImage2D(type, 0, format, width, height, 0, fileFormat, GL_UNSIGNED_BYTE, data));
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

    return std::unique_ptr<Texture>(new Texture(texture, name, type, format));
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
