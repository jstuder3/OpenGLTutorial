#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vertex, fragment;
	vertex = loadAndCompileShader(vertexPath, GL_VERTEX_SHADER);
	fragment = loadAndCompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//print linking errors if any
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath) {
	unsigned int vertex, geometry, fragment;
	vertex = loadAndCompileShader(vertexPath, GL_VERTEX_SHADER);
	geometry = loadAndCompileShader(geometryPath, GL_GEOMETRY_SHADER);
	fragment = loadAndCompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, geometry);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//print linking errors if any
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(geometry); 
	glDeleteShader(fragment);

}

unsigned int Shader::loadAndCompileShader(const char* path, GLenum shaderType) {
	// 1. retrieve the vertex / fragment source code from filePath
	std::string shaderCode;
	std::ifstream shaderFile;
	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		//open file
		shaderFile.open(path);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		//close file handlers
		shaderFile.close();
		//convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		std::cout << "ERRONEOUS FILE: " << path << "\n";
	}

	const char* cShaderCode = shaderCode.c_str();

	// allocate and compile shader
	unsigned int shaderID;
	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &cShaderCode, NULL);
	glCompileShader(shaderID);

	// print compile errors if any
	checkCompileErrors(shaderID, "GEOMETRY");
	return shaderID;
}

void Shader::use() {
	glUseProgram(ID);
	hasUsed = true;
}

void Shader::setBool(const std::string& name, bool value) const {
	checkUseBeforeSettingUniform();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);

}

void Shader::setInt(const std::string& name, int value) const {
	checkUseBeforeSettingUniform();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setTexture(const std::string& name, int textureUnit, int textureID) const {
	checkUseBeforeSettingUniform();
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(ID, name.c_str()), textureUnit);
}

void Shader::setFloat(const std::string& name, float value) const {
	checkUseBeforeSettingUniform();
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	checkUseBeforeSettingUniform();
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	checkUseBeforeSettingUniform();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const {
	checkUseBeforeSettingUniform();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(glm::vec3(x, y, z)));
}


void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	checkUseBeforeSettingUniform();
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::deleteShader() const {
	glDeleteProgram(ID);
}

void Shader::checkUseBeforeSettingUniform() const {
	if (!hasUsed) {
		std::cout << "ERROR::SHADER::MUST_USE_SHADER_BEFORE_SETTING_UNIFORM" << std::endl;
		exit(1);
	}
}

void Shader::checkCompileErrors(GLuint shader, std::string type) const {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
