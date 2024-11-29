#pragma once

#include <string>
#include <glm/glm.hpp>

class ShaderProgram {
public:
	ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
	~ShaderProgram();

	void use() const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, float value) const;
	void setUniform(const std::string& name, const glm::mat4& matrix) const;
	void setUniform(const std::string& name, const glm::vec3& vector) const;
	void setUniform(const std::string& name, bool value) const;



private:
	unsigned int programID;
	unsigned int compileShader(const std::string& source, unsigned int type);
	std::string loadShaderSource(const std::string& filepath);
	void checkCompileErrors(unsigned int shader, const std::string& type);
	void checkLinkErrors();
};



