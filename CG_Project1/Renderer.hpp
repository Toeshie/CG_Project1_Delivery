#pragma once

#include "ShaderProgram.hpp"
#include "OBJLoader.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "Character.hpp"

class Renderer {
public:
	enum class WeaponType {
		RIFLE,
		PISTOL,
		KNIFE,
		NONE
	};

	Renderer();
	~Renderer();

	bool initialize(const OBJLoader& objLoader);
	void render(const ShaderProgram& shaderProgram, const OBJLoader& objLoader);
	void cleanup();
	bool initializeWeapons(const OBJLoader& rifleLoader, const OBJLoader& pistolLoader, const OBJLoader& knifeLoader);
	void renderWeapon(const ShaderProgram& shaderProgram, WeaponType currentWeapon);
	bool initializeCharacterModels(const OBJLoader& ctLoader, const OBJLoader& tLoader);
	void renderCharacter(const ShaderProgram& shaderProgram, const Character& character);

private:
	struct MaterialBuffers {
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
	};
	std::vector<MaterialBuffers> materialBuffers;
	std::vector<MaterialBuffers> rifleBuffers;
	std::vector<MaterialBuffers> pistolBuffers;
	std::vector<MaterialBuffers> knifeBuffers;
	OBJLoader ctModel;
	OBJLoader tModel;
	std::vector<MaterialBuffers> ctBuffers;
	std::vector<MaterialBuffers> tBuffers;

	void setupBuffers(const OBJLoader& objLoader);
	void setupWeaponBuffers(const OBJLoader& objLoader, std::vector<MaterialBuffers>& buffers);

	OBJLoader rifleLoader;
	OBJLoader pistolLoader;
	OBJLoader knifeLoader;
};



