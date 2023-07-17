#pragma once

#include <iostream>
#include <memory>

class Texture{
private:
    Texture(unsigned int id, unsigned int type, unsigned int format);

public:
    static std::unique_ptr<Texture> CreateTexture(const char* path, unsigned int type, unsigned int format);
    void SetWrapping(unsigned int wrap_s, unsigned int wrap_t);
    void SetFiltering(unsigned int minFilter, unsigned int magFilter);
    void Bind();
    void BindAndActive(unsigned int unit);
    void Unbind();
    ~Texture();

private:
    unsigned int _type;
    unsigned int _format;
    unsigned int _renderID;
};
