#pragma once
#include <unordered_map>
#include <string>

#include <mygl/Texture.hpp>


class TextureCache
{
public:
    static TextureCache& getInstance()
    {
        static TextureCache instance;
        return instance;
    }
    TextureCache(TextureCache const&) = delete;
    void operator=(TextureCache const&) = delete;

    bool HasTexture(const std::string path);
    void LoadTexture(const std::string path);
    void UnloadTexture(const std::string path);
    void Clear();
    Texture& GetTexture(const std::string path);
    unsigned int NumTextures();

private:
    std::unordered_map<std::string, Texture> *loadedTextures;
    TextureCache() {loadedTextures = new std::unordered_map<std::string, Texture>();}

};
