#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>

class Texture{
private:
    Texture(unsigned int id, std::string name, unsigned int type, unsigned int format);

public:
    void SetWrapping(unsigned int wrap_s, unsigned int wrap_t);
    void SetFiltering(unsigned int minFilter, unsigned int magFilter);
    void SetBorderColor(const float*data);
    void Bind();
    void BindAndActive(unsigned int unit);
    void Unbind();
    unsigned int GetRenderID() const { return _renderID; }
    const std::string& GetName() const { return _name;}
    ~Texture();

    friend class TextureManager;

private:
    unsigned int _type;
    unsigned int _format;
    unsigned int _renderID;
    std::string _name;
};

class TextureManager {
public:

    static void Init();
    static void Clear();
    static Texture* GetTexture(int id);
    static Texture* GetTexture(const std::string& fullName);
    static int LoadTexture(const std::string& fullName, unsigned int type);
    static int LoadCubeMap(const std::string&name, const std::vector<std::string>& facesPath);
    static int CreateTexture(const std::string& texName, unsigned int target, unsigned int format, unsigned int w, unsigned int h, unsigned int type = 0, void* data = nullptr);
    static int GetTextureIndex(const std::string& fullName);
    static int GetTextureRenderID(int id);
    static bool IsLoaded(const std::string& fullName);

private:
    TextureManager(){}
    int id{0};
    static std::unique_ptr<TextureManager> instance;
    static bool isInit;
    std::map<std::string, int> _generatedMap;
    std::unordered_map<int, std::unique_ptr<Texture>> _textures;
};
