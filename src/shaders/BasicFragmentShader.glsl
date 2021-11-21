#version 460 core
in vec3 vColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    FragColor = texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(vColor, 1.0);
}