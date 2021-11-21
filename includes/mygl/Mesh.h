#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <mygl/Shader.h>
#include <mygl/Texture.hpp>


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum TextureType
{
    diffuse,
    specular
};

struct TextureID
{
    Texture &texture;
    TextureType type;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureID> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures);
    void Draw(Shader &shader);

private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};
