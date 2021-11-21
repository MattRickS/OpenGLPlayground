#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <mygl/Lights.h>


std::string LoadFile(const char* filename);
GLuint CompileShader(const char* source, GLenum shaderType);
GLuint CompileProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint LoadShadersProgram(const char* vertexShaderFile, const char* fragmetnShaderFile);

class Shader
{
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	// Utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat3(const std::string& name, float x, float y, float z) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;
	void setFloat4(const std::string& name, float x, float y, float z, float w) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;
	void setPointLight(const std::string& name, PointLight light) const;
	void setPointLight(const std::string& name, PointLight light, unsigned int index) const;
	void setSpotLight(const std::string& name, SpotLight light) const;
	void setSpotLight(const std::string& name, SpotLight light, unsigned int index) const;
	void setDirLight(const std::string& name, DirLight light) const;
	void setDirLight(const std::string& name, DirLight light, unsigned int index) const;
private:
	GLuint getLocation(const std::string& name) const;
};