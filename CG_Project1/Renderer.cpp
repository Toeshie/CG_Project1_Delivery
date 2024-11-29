#include "Renderer.hpp"
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer() {}

Renderer::~Renderer() {
	cleanup();
}

bool Renderer::initialize(const OBJLoader& objLoader) {
	setupBuffers(objLoader);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	return true;
}

void Renderer::setupBuffers(const OBJLoader& objLoader) {
	const auto& materials = objLoader.getMaterials();
	materialBuffers.resize(materials.size());

	for (size_t i = 0; i < materials.size(); i++) {
		const auto& material = materials[i];
		auto& buffers = materialBuffers[i];

		std::vector<float> vertexData;
		for (size_t j = 0; j < material.vertices.size(); j++) {
			// Position
			vertexData.push_back(material.vertices[j].x);
			vertexData.push_back(material.vertices[j].y);
			vertexData.push_back(material.vertices[j].z);

			// UV coordinates
			if (j < material.uvs.size()) {
				vertexData.push_back(material.uvs[j].x);
				vertexData.push_back(material.uvs[j].y);
			}
			else {
				vertexData.push_back(0.0f);
				vertexData.push_back(0.0f);
			}
		}

		glGenVertexArrays(1, &buffers.VAO);
		glGenBuffers(1, &buffers.VBO);
		glGenBuffers(1, &buffers.EBO);

		glBindVertexArray(buffers.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, material.indices.size() * sizeof(unsigned int),
			material.indices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}

void Renderer::render(const ShaderProgram& shaderProgram, const OBJLoader& objLoader) {
	shaderProgram.use();
	const auto& materials = objLoader.getMaterials();

	for (size_t i = 0; i < materials.size(); i++) {
		const auto& material = materials[i];
		const auto& buffers = materialBuffers[i];

		if (material.textureID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material.textureID);
			shaderProgram.setUniform("texture1", 0);

			glBindVertexArray(buffers.VAO);
			glDrawElements(GL_TRIANGLES,
				static_cast<GLsizei>(material.indices.size()),
				GL_UNSIGNED_INT,
				0);
			glBindVertexArray(0);
		}
	}
}

void Renderer::cleanup() {
	for (const auto& buffers : materialBuffers) {
		glDeleteVertexArrays(1, &buffers.VAO);
		glDeleteBuffers(1, &buffers.VBO);
		glDeleteBuffers(1, &buffers.EBO);
	}
	materialBuffers.clear();
}

bool Renderer::initializeWeapons(const OBJLoader& rifleLoader, const OBJLoader& pistolLoader, const OBJLoader& knifeLoader) {
	// Store the loaders
	this->rifleLoader = rifleLoader;
	this->pistolLoader = pistolLoader;
	this->knifeLoader = knifeLoader;

	// Initialize rifle
	const auto& rifleMaterials = this->rifleLoader.getMaterials();
	rifleBuffers.resize(rifleMaterials.size());
	setupWeaponBuffers(this->rifleLoader, rifleBuffers);

	// Initialize pistol
	const auto& pistolMaterials = this->pistolLoader.getMaterials();
	pistolBuffers.resize(pistolMaterials.size());
	setupWeaponBuffers(this->pistolLoader, pistolBuffers);

	// Initialize knife
	const auto& knifeMaterials = this->knifeLoader.getMaterials();
	knifeBuffers.resize(knifeMaterials.size());
	setupWeaponBuffers(this->knifeLoader, knifeBuffers);

	return true;
}

void Renderer::renderWeapon(const ShaderProgram& shaderProgram, WeaponType currentWeapon) {
	//std::cout << "\n=== Weapon Render Debug ===\n";
	//std::cout << "Rendering weapon type: " << static_cast<int>(currentWeapon) << std::endl;
	
	// Save current OpenGL state
	GLboolean depthTest, cullFace, blend;
	glGetBooleanv(GL_DEPTH_TEST, &depthTest);
	glGetBooleanv(GL_CULL_FACE, &cullFace);
	glGetBooleanv(GL_BLEND, &blend);

	// Setup OpenGL state for weapon rendering
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);  // Disable face culling for all weapons
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const std::vector<MaterialBuffers>* weaponBuffers = nullptr;
	const std::vector<Material>* materials = nullptr;
	
	switch(currentWeapon) {
		case WeaponType::RIFLE:
			//std::cout << "Selected RIFLE buffers. Size: " << rifleBuffers.size() << std::endl;
			weaponBuffers = &rifleBuffers;
			materials = &rifleLoader.getMaterials();
			break;
		case WeaponType::PISTOL:
			//std::cout << "Selected PISTOL buffers. Size: " << pistolBuffers.size() << std::endl;
			weaponBuffers = &pistolBuffers;
			materials = &pistolLoader.getMaterials();
			break;
		case WeaponType::KNIFE:
			//std::cout << "Selected KNIFE buffers. Size: " << knifeBuffers.size() << std::endl;
			weaponBuffers = &knifeBuffers;
			materials = &knifeLoader.getMaterials();
			break;
		default:
			std::cout << "Invalid weapon type!" << std::endl;
			return;
	}

	if (!weaponBuffers || !materials) {
		std::cout << "Error: Null buffers or materials!" << std::endl;
		return;
	}

	//std::cout << "Number of materials to render: " << materials->size() << std::endl;

	// Render the weapon
	for (size_t i = 0; i < materials->size(); i++) {
		const auto& material = (*materials)[i];
		const auto& buffers = (*weaponBuffers)[i];
		
		if (material.textureID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material.textureID);
			shaderProgram.setUniform("texture1", 0);

			glBindVertexArray(buffers.VAO);
			
			// For knife, render both front and back faces
			if (currentWeapon == WeaponType::KNIFE) {
				// First pass: render back faces
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glDrawElements(GL_TRIANGLES,
					static_cast<GLsizei>(material.indices.size()),
					GL_UNSIGNED_INT,
					0);

				// Second pass: render front faces
				glCullFace(GL_BACK);
				glDrawElements(GL_TRIANGLES,
					static_cast<GLsizei>(material.indices.size()),
					GL_UNSIGNED_INT,
					0);
				glDisable(GL_CULL_FACE);
			} else {
				// Normal rendering for other weapons
				glDrawElements(GL_TRIANGLES,
					static_cast<GLsizei>(material.indices.size()),
					GL_UNSIGNED_INT,
					0);
			}
			
			glBindVertexArray(0);
		}
	}

	// Restore previous OpenGL state
	if (!depthTest) glDisable(GL_DEPTH_TEST);
	if (cullFace) glEnable(GL_CULL_FACE);
	if (!blend) glDisable(GL_BLEND);
	glCullFace(GL_BACK);  // Reset to default culling

	//std::cout << "=== End Weapon Render Debug ===\n";
}

void Renderer::setupWeaponBuffers(const OBJLoader& objLoader, std::vector<MaterialBuffers>& buffers) {
	const auto& materials = objLoader.getMaterials();

	for (size_t i = 0; i < materials.size(); i++) {
		const auto& material = materials[i];
		auto& buffer = buffers[i];

		std::vector<float> vertexData;
		for (size_t j = 0; j < material.vertices.size(); j++) {
			// Position
			vertexData.push_back(material.vertices[j].x);
			vertexData.push_back(material.vertices[j].y);
			vertexData.push_back(material.vertices[j].z);

			// UV coordinates
			if (j < material.uvs.size()) {
				vertexData.push_back(material.uvs[j].x);
				vertexData.push_back(material.uvs[j].y);
			}
			else {
				vertexData.push_back(0.0f);
				vertexData.push_back(0.0f);
			}
		}

		glGenVertexArrays(1, &buffer.VAO);
		glGenBuffers(1, &buffer.VBO);
		glGenBuffers(1, &buffer.EBO);

		glBindVertexArray(buffer.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, material.indices.size() * sizeof(unsigned int),
			material.indices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}

bool Renderer::initializeCharacterModels(const OBJLoader& ctLoader, const OBJLoader& tLoader) {
	// Store the loaders
	this->ctModel = ctLoader;
	this->tModel = tLoader;

	// Initialize CT model buffers
	const auto& ctMaterials = this->ctModel.getMaterials();
	ctBuffers.resize(ctMaterials.size());
	setupWeaponBuffers(this->ctModel, ctBuffers); // We can reuse this method

	// Initialize T model buffers
	const auto& tMaterials = this->tModel.getMaterials();
	tBuffers.resize(tMaterials.size());
	setupWeaponBuffers(this->tModel, tBuffers);

	return true;
}

void Renderer::renderCharacter(const ShaderProgram& shaderProgram, const Character& character) {
	const std::vector<MaterialBuffers>* characterBuffers;
	const std::vector<Material>* materials;

	if (character.team == Character::Team::CT) {
		characterBuffers = &ctBuffers;
		materials = &ctModel.getMaterials();
	} else {
		characterBuffers = &tBuffers;
		materials = &tModel.getMaterials();
	}

	// Get the model matrix from the character
	glm::mat4 model = character.getModelMatrix();
	
	// Set all matrices
	shaderProgram.setUniform("model", model);
	// Note: view and projection matrices should be set before calling this function
	// They are set in the main render loop

	for (size_t i = 0; i < materials->size(); i++) {
		const auto& material = (*materials)[i];
		const auto& buffers = (*characterBuffers)[i];

		if (material.textureID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material.textureID);
			shaderProgram.setUniform("texture1", 0);

			glBindVertexArray(buffers.VAO);
			glDrawElements(GL_TRIANGLES,
				static_cast<GLsizei>(material.indices.size()),
				GL_UNSIGNED_INT,
				0);
			glBindVertexArray(0);
		}
	}
}




