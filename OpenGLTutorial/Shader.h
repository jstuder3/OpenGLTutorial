#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
class Shader
{
public:
	// the program ID
	unsigned int ID;
	bool hasUsed = false;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
	unsigned int loadAndCompileShader(const char* shaderPath, GLenum shaderType);
	//use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setTexture(const std::string& name, int textureUnit, int textureID) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, const float x, const float y, const float z) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void deleteShader() const;
	void checkCompileErrors(GLuint shader, std::string type) const;
	void checkUseBeforeSettingUniform() const;
};

#endif
