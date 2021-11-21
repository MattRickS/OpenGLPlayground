#include <mygl/TextureCache.hpp>

// std::unordered_map<std::string, Texture> TextureCache::loadedTextures;


bool TextureCache::HasTexture(const std::string path)
{
    return loadedTextures->find(path) != loadedTextures->end();
}

void TextureCache::LoadTexture(const std::string path)
{
    // loadedTextures[path] = path.c_str();
    // loadedTextures[path] = Texture(path.c_str());
    loadedTextures->insert({path, Texture(path.c_str())});
    // loadedTextures->emplace(path, path.c_str());
    // loadedTextures->emplace(path, Texture(path.c_str()));
}

void TextureCache::UnloadTexture(const std::string path)
{
    loadedTextures->erase(path);
}

void TextureCache::Clear()
{
    loadedTextures->clear();
}

Texture& TextureCache::GetTexture(const std::string path)
{
    return loadedTextures->at(path);
    // return loadedTextures[path];
}

unsigned int TextureCache::NumTextures()
{
    return loadedTextures->size();
}


