#version 330 core

struct Material
{
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
}; 

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform Material material;

void main()
{
    FragColor = vec4(vec3(texture(material.diffuse1, TexCoord)), 1.0);
}
