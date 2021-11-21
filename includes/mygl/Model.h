#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include <assimp/scene.h>

#include <mygl/Mesh.h>
#include <mygl/Shader.h>


class Model
{
public:
    Model(const char *path);
    void Draw(Shader &shader);
private:
    std::vector<Mesh> meshes;
    std::filesystem::path directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::string getTexturePath(const char *texturePath) const;
    void loadMaterialTextures(std::vector<TextureID> &textures, aiMaterial *mat, aiTextureType type, TextureType textureType);
};
