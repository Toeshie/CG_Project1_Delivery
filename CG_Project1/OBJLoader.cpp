#include "OBJLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <stb_image.h>

OBJLoader::OBJLoader() : vertices(), uvs(), normals(), indices(), materials() {}

bool OBJLoader::loadOBJ(const std::string& path) {
	std::ifstream objFile(path);
	if (!objFile.is_open()) {
		std::cerr << "Failed to open OBJ file: " << path << std::endl;
		return false;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	Material* currentMaterial = nullptr;

	std::string line;
	while (std::getline(objFile, line)) {
		std::istringstream lineStream(line);
		std::string prefix;
		lineStream >> prefix;

		if (prefix == "mtllib") {
			std::string mtlFile;
			lineStream >> mtlFile;
			std::string mtlPath = path.substr(0, path.find_last_of("/\\") + 1) + mtlFile;
			if (!loadMTL(mtlPath)) {
				std::cerr << "Failed to load MTL file: " << mtlPath << std::endl;
				return false;
			}
		}
		else if (prefix == "v") {
			glm::vec3 vertex;
			lineStream >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (prefix == "vt") {
			glm::vec2 uv;
			lineStream >> uv.x >> uv.y;
			// Remove the flip
			temp_uvs.push_back(uv);
		}
		else if (prefix == "vn") {
			glm::vec3 normal;
			lineStream >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (prefix == "usemtl") {
			std::string materialName;
			lineStream >> materialName;
			currentMaterial = nullptr;
			for (auto& material : materials) {
				if (material.name == materialName) {
					currentMaterial = &material;
					break;
				}
			}
		}
		else if (prefix == "f") {
			if (!currentMaterial) {
				std::cerr << "Warning: Face defined before material" << std::endl;
				continue;
			}

			std::vector<std::string> vertexData;
			std::string vertex;
			while (lineStream >> vertex) {
				vertexData.push_back(vertex);
			}

			// Triangulate the face (handle both triangles and quads)
			for (size_t i = 2; i < vertexData.size(); i++) {
				// For each triangle, process three vertices: 0, i-1, i
				std::vector<size_t> triangleIndices = {0, i-1, i};
				
				for (size_t idx : triangleIndices) {
					std::istringstream vertexStream(vertexData[idx]);
					std::string vertexIndex, uvIndex, normalIndex;
					
					std::getline(vertexStream, vertexIndex, '/');
					std::getline(vertexStream, uvIndex, '/');
					std::getline(vertexStream, normalIndex);

					int vIdx = std::stoi(vertexIndex) - 1;
					int uvIdx = uvIndex.empty() ? -1 : std::stoi(uvIndex) - 1;

					if (vIdx < 0 || static_cast<size_t>(vIdx) >= temp_vertices.size()) {
						std::cerr << "Warning: Invalid vertex index" << std::endl;
						continue;
					}

					currentMaterial->vertices.push_back(temp_vertices[vIdx]);
					
					if (uvIdx >= 0 && static_cast<size_t>(uvIdx) < temp_uvs.size()) {
						currentMaterial->uvs.push_back(temp_uvs[uvIdx]);
					} else {
						currentMaterial->uvs.push_back(glm::vec2(0.0f, 0.0f));
					}
					
					currentMaterial->indices.push_back(static_cast<unsigned int>(currentMaterial->vertices.size() - 1));
				}
			}
		}
	}

	objFile.close();
	return true;
}

bool OBJLoader::loadMTL(const std::string& mtlPath) {
	std::ifstream mtlFile(mtlPath);
	if (!mtlFile.is_open()) {
		std::cerr << "Failed to open MTL file: " << mtlPath << std::endl;
		return false;
	}

	std::string line;
	Material material;
	while (std::getline(mtlFile, line)) {
		std::istringstream lineStream(line);
		std::string prefix;
		lineStream >> prefix;

		if (prefix == "newmtl") {
			if (!material.name.empty()) {
				materials.push_back(material);
				material = Material();
			}
			lineStream >> material.name;
			std::cout << "Loading material: " << material.name << std::endl;
		}
		else if (prefix == "map_Kd") {
			std::string texturePath;
			lineStream >> texturePath;

			if (texturePath.find("Assets/") != 0) {
				std::string baseDir = mtlPath.substr(0, mtlPath.find_last_of("/\\") + 1);
				texturePath = baseDir + texturePath;
			}

			material.textureFilename = texturePath;
			material.textureID = loadTexture(texturePath);

			if (material.textureID != 0) {
				std::cout << "Successfully loaded texture for material " << material.name
					<< " from path: " << texturePath
					<< " with ID: " << material.textureID << std::endl;
			}
			else {
				std::cerr << "Failed to load texture for material " << material.name
					<< " from path: " << texturePath << std::endl;
			}
		}
	}

	if (!material.name.empty()) {
		materials.push_back(material);
	}

	mtlFile.close();
	return true;
}

unsigned int OBJLoader::loadTexture(const std::string& textureFilename) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(textureFilename.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Successfully loaded texture: " << textureFilename
			<< " with ID: " << textureID
			<< " (Dimensions: " << width << "x" << height
			<< ", Channels: " << nrChannels << ")" << std::endl;
	}
	else {
		std::cerr << "Failed to load texture: " << textureFilename << std::endl;
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	stbi_image_free(data);

	return textureID;
}

const std::vector<glm::vec3>& OBJLoader::getVertices() const {
	return vertices;
}

const std::vector<glm::vec2>& OBJLoader::getUVs() const {
	return uvs;
}

const std::vector<Material>& OBJLoader::getMaterials() const {
	return materials;
}

const std::vector<unsigned int>& OBJLoader::getIndices() const {
	return indices;
}