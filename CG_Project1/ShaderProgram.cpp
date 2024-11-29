#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
	std::string vertexCode = loadShaderSource(vertexPath);
	std::string fragmentCode = loadShaderSource(fragmentPath);

	unsigned int vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
	unsigned int fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	checkLinkErrors();

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(programID);
}

void ShaderProgram::use() const {
	glUseProgram(programID);
}

unsigned int ShaderProgram::compileShader(const std::string& source, unsigned int type) {
	unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
	return shader;
}

std::string ShaderProgram::loadShaderSource(const std::string& filepath) {
	std::ifstream file(filepath);
	std::stringstream buffer;
	if (file) {
		buffer << file.rdbuf();
	}
	else {
		std::cerr << "Error: Could not open shader file " << filepath << std::endl;
	}
	return buffer.str();
}

void ShaderProgram::checkCompileErrors(unsigned int shader, const std::string& type) {
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
	}
}

void ShaderProgram::checkLinkErrors() {
	int success;
	char infoLog[1024];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
		std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
	}
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& matrix) const {
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& vector) const {
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &vector[0]);
}

void ShaderProgram::setUniform(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
}

