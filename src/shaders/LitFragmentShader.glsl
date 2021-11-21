#version 330 core
out vec4 FragColor;
  
uniform float ambientStrength = 0.1f;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor

    FragColor = vec4(result, 1.0);
}