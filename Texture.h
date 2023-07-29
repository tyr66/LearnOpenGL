#pragma once

#include <iostream>
#include <memory>
#include <string>


class Texture{
private:
    Texture(unsigned int id, std::string name, unsigned int type, unsigned int format);

public:
    static std::unique_ptr<Texture> CreateTexture(std::string path, std::string name, unsigned int type, unsigned int format);
    void SetWrapping(unsigned int wrap_s, unsigned int wrap_t);
    void SetFiltering(unsigned int minFilter, unsigned int magFilter);
    void Bind();
    void BindAndActive(unsigned int unit);
    void Unbind();
    unsigned int GetRenderID() const { return _renderID; }
    const std::string& GetName() const { return _name;}
    ~Texture();

private:
    unsigned int _type;
    unsigned int _format;
    unsigned int _renderID;
    std::string _name;
};
