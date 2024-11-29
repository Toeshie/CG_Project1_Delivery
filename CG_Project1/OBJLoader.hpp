#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

struct Material {
	std::string name;
	std::string textureFilename;
	unsigned int textureID;
	std::vector<unsigned int> indices;  // Store indices for each material
	std::vector<glm::vec3> vertices;    // Store vertices for each material
	std::vector<glm::vec2> uvs;         // Store UVs for each material

	Material() : name(), textureFilename(), textureID(0), indices(), vertices(), uvs() {}
};

class OBJLoader {
public:
	OBJLoader();
	bool loadOBJ(const std::string& path);
	const std::vector<glm::vec3>& getVertices() const;
	const std::vector<glm::vec2>& getUVs() const;
	const std::vector<Material>& getMaterials() const;
	const std::vector<unsigned int>& getIndices() const;

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<Material> materials;

	bool loadMTL(const std::string& path);
	unsigned int loadTexture(const std::string& textureFilename);
};
