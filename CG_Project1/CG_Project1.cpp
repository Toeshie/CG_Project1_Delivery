#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "OBJLoader.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Skybox.hpp"
#include "Crosshair.hpp"
#include "Character.hpp"
#include <vector>
#include <random>
#include <ctime>

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Weapon ENUM for weapon swaping
Renderer::WeaponType currentWeapon = Renderer::WeaponType::KNIFE;

// Camera movement speeds
const float RIFLE_SPEED = 7.0f;    // Slowest
const float PISTOL_SPEED = 8.0f;   // Medium
const float KNIFE_SPEED = 10.0f;    // Fastest

int main() {
	// Initialize the window
	WindowManager window("CG_Project1", 1366, 768);
	if (!window.initialize()) return -1;

	// Load the map
	OBJLoader mapLoader;
	if (!mapLoader.loadOBJ("Assets/Dust2/Dust2.obj")) {
		std::cerr << "Failed to load map model." << std::endl;
		return -1;
	}

	// Load all weapons
	OBJLoader rifleLoader, pistolLoader, knifeLoader;
	
	if (!rifleLoader.loadOBJ("Assets/AK/AK47.obj")) {
		std::cerr << "Failed to load rifle model." << std::endl;
		return -1;
	}
	std::cout << "Rifle loaded successfully. Materials: " << rifleLoader.getMaterials().size() << std::endl;
	
	if (!pistolLoader.loadOBJ("Assets/USP/USP.obj")) {
		std::cerr << "Failed to load pistol model." << std::endl;
		return -1;
	}
	std::cout << "Pistol loaded successfully. Materials: " << pistolLoader.getMaterials().size() << std::endl;
	
	if (!knifeLoader.loadOBJ("Assets/Knife/knife.obj")) {
		std::cerr << "Failed to load knife model." << std::endl;
		return -1;
	}
	std::cout << "Knife loaded successfully. Materials: " << knifeLoader.getMaterials().size() << std::endl;

	// Initialize the renderer with all models
	Renderer renderer;
	if (!renderer.initialize(mapLoader) || 
		!renderer.initializeWeapons(rifleLoader, pistolLoader, knifeLoader)) {
		std::cerr << "Failed to initialize renderer." << std::endl;
		return -1;
	}

	// Load and use the shader program
	ShaderProgram shader("VertexShader.glsl", "FragmentShader.glsl");
	shader.use();

	// Set up camera
	Camera camera(glm::vec3(-18.0f, 4.21f, 18.0f));
	camera.MovementSpeed = KNIFE_SPEED;  // Set initial speed to match starting weapon (KNIFE)
	bool running = true;
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Initialize skybox
	std::vector<std::string> faces{
		"Assets/Skybox/Daylight Box_Right.bmp",
		"Assets/Skybox/Daylight Box_Left.bmp",
		"Assets/Skybox/Daylight Box_Top.bmp",
		"Assets/Skybox/Daylight Box_Bottom.bmp",
		"Assets/Skybox/Daylight Box_Front.bmp",
		"Assets/Skybox/Daylight Box_Back.bmp"
	};

	Skybox skybox;
	if (!skybox.initialize(faces)) {
		std::cerr << "Failed to initialize skybox." << std::endl;
		return -1;
	}

	ShaderProgram skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");

	// Initialize crosshair
	Crosshair crosshair(1366.0f, 768.0f);

	// Load character models
	OBJLoader ctModelLoader, tModelLoader;
	if (!ctModelLoader.loadOBJ("Assets/Players/CT/CT.obj")) {
		std::cerr << "Failed to load CT model." << std::endl;
		return -1;
	}
	if (!tModelLoader.loadOBJ("Assets/Players/T/T.obj")) {
		std::cerr << "Failed to load T model." << std::endl;
		return -1;
	}

	// Initialize character models in renderer
	if (!renderer.initializeCharacterModels(ctModelLoader, tModelLoader)) {
		std::cerr << "Failed to initialize character models." << std::endl;
		return -1;
	}

	// Create characters with random offsets
	std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
	std::uniform_real_distribution<float> posOffset(-1.0f, 1.0f);
	std::uniform_real_distribution<float> rotOffset(-180.0f, 180.0f);

	std::vector<Character> characters;

	// CT positions (CT spawn area)
	glm::vec3 ctPositions[] = {
		glm::vec3(8.0f, -3.18f, -59.0f),
		glm::vec3(7.0f, -3.18f, -57.0f),
		glm::vec3(6.0f, -3.18f, -58.0f),
		glm::vec3(9.0f, -3.18f, -56.0f),
		glm::vec3(5.0f, -3.18f, -60.0f)
	};

	// T positions (T spawn area)
	glm::vec3 tPositions[] = {
		glm::vec3(-15.0f, 3.21f, 20.0f),
		glm::vec3(-17.0f, 3.21f, 21.0f),
		glm::vec3(-19.0f, 3.21f, 19.0f),
		glm::vec3(-18.0f, 3.21f, 18.0f)
	};

	// Spawn CTs
	for (const auto& basePos : ctPositions) {
		glm::vec3 offsetPos = basePos + glm::vec3(posOffset(rng), 0.0f, posOffset(rng));
		float offsetRot = rotOffset(rng);
		characters.emplace_back(offsetPos, Character::Team::CT, offsetRot);
	}

	// Spawn Ts
	for (const auto& basePos : tPositions) {
		glm::vec3 offsetPos = basePos + glm::vec3(posOffset(rng), 0.0f, posOffset(rng));
		float offsetRot = rotOffset(rng);
		characters.emplace_back(offsetPos, Character::Team::T, offsetRot);
	}

	while (running) {
		float currentFrame = SDL_GetTicks() / 1000.0f;
		
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;
			if (event.type == SDL_MOUSEWHEEL) camera.ProcessMouseScroll(static_cast<float>(event.wheel.y));
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_1:
						currentWeapon = Renderer::WeaponType::RIFLE;
						camera.MovementSpeed = RIFLE_SPEED;
						std::cout << "Switched to RIFLE" << std::endl;
						break;
					case SDLK_2:
						currentWeapon = Renderer::WeaponType::PISTOL;
						camera.MovementSpeed = PISTOL_SPEED;
						std::cout << "Switched to PISTOL" << std::endl;
						break;
					case SDLK_3:
						currentWeapon = Renderer::WeaponType::KNIFE;
						camera.MovementSpeed = KNIFE_SPEED;
						std::cout << "Switched to KNIFE" << std::endl;
						break;
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_e:
						camera.toggleYLock();
						std::cout << "Camera Y-Lock: " << (camera.isYLocked ? "Enabled" : "Disabled") << std::endl;
						break;
				}
			}
		}

		// Camera movement
		const Uint8* state = SDL_GetKeyboardState(nullptr);
		glm::vec3 movement(0.0f);
		if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_S] || 
			state[SDL_SCANCODE_A] || state[SDL_SCANCODE_D]) {
			
			if (camera.isYLocked) {
				// When Y-locked, use a flattened front vector for forward/backward movement
				glm::vec3 flatFront = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
				glm::vec3 flatRight = glm::normalize(glm::vec3(camera.Right.x, 0.0f, camera.Right.z));
				
				if (state[SDL_SCANCODE_W]) movement += flatFront;
				if (state[SDL_SCANCODE_S]) movement -= flatFront;
				if (state[SDL_SCANCODE_A]) movement -= flatRight;
				if (state[SDL_SCANCODE_D]) movement += flatRight;
			} else {
				// Normal movement when not Y-locked
				if (state[SDL_SCANCODE_W]) movement += camera.Front;
				if (state[SDL_SCANCODE_S]) movement -= camera.Front;
				if (state[SDL_SCANCODE_A]) movement -= camera.Right;
				if (state[SDL_SCANCODE_D]) movement += camera.Right;
			}

			// Normalize and apply movement
			if (glm::length(movement) > 0.0f) {
				movement = glm::normalize(movement);
				glm::vec3 newPosition = camera.Position + movement * (camera.MovementSpeed * deltaTime);
				
				if (camera.isYLocked) {
					newPosition.y = camera.lockedY;  // Maintain the locked Y position
				}
				
				camera.Position = newPosition;
			}
		}

		// Mouse motion for camera rotation
		int xrel, yrel;
		SDL_GetRelativeMouseState(&xrel, &yrel);
		camera.ProcessMouseMovement(static_cast<float>(xrel), static_cast<float>(-yrel));

		// Clear the screen and depth buffer
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Set up view and projection matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		
		// Map model matrix
		glm::mat4 mapModel = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));	
		
		
		// Update weapon positioning
		glm::vec3 weaponPosition;
		float weaponScale;
		glm::vec3 weaponRotationAngles;

		switch (currentWeapon) {
			case Renderer::WeaponType::RIFLE:
				weaponPosition = glm::vec3(0.4f, -0.3f, -0.6f);
				weaponScale = 0.005f;
				weaponRotationAngles = glm::vec3(0.0f, 180.0f, 0.0f);
				break;
			case Renderer::WeaponType::PISTOL:
				weaponPosition = glm::vec3(0.18f, -0.2f, -0.6f);
				weaponScale = 0.03f;
				weaponRotationAngles = glm::vec3(0.0f, -90.0f, 0.0f);
				break;
			case Renderer::WeaponType::KNIFE:
				weaponPosition = glm::vec3(0.1f, -0.2f, -0.5f);
				weaponScale = 0.1f;
				weaponRotationAngles = glm::vec3(0.0f, 180.0f, 15.0f);
				break;
		}

		glm::mat4 weaponModel = glm::mat4(1.0f);
		weaponModel = glm::translate(weaponModel, weaponPosition);
		
		weaponModel = glm::rotate(weaponModel, glm::radians(weaponRotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		weaponModel = glm::rotate(weaponModel, glm::radians(weaponRotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		weaponModel = glm::rotate(weaponModel, glm::radians(weaponRotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

		if (currentWeapon == Renderer::WeaponType::KNIFE) {
			float bobbingOffset = sin(currentFrame * 2.0f) * 0.02f;
			weaponModel = glm::translate(weaponModel, glm::vec3(0.0f, bobbingOffset, 0.0f));
		}

		weaponModel = glm::scale(weaponModel, glm::vec3(weaponScale));

		// Add slight tilt for better viewing angle (common for all weapons)
		weaponModel = glm::rotate(weaponModel, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// Only apply pitch rotation (vertical movement)
		float pitch = camera.Pitch * 0.1f; 
		glm::mat4 weaponRotation = glm::mat4(1.0f);
		weaponRotation = glm::rotate(weaponRotation, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

		// Final transformation
		glm::mat4 weaponTransform = projection * weaponModel;

		// debug output
		//std::cout << "Camera Position: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
		//std::cout << "Camera Front: " << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << std::endl;

		// Draw skybox first
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove translation from the view matrix
		skyboxShader.use();
		skyboxShader.setUniform("view", skyboxView);
		skyboxShader.setUniform("projection", projection);
		skybox.render(skyboxShader);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Render the map
		shader.use();
		shader.setUniform("model", mapModel);
		shader.setUniform("view", view);
		shader.setUniform("projection", projection);
		renderer.render(shader, mapLoader);

		// Render characters
		for (const auto& character : characters) {
			renderer.renderCharacter(shader, character);
		}

		// Render the weapon
		shader.setUniform("model", weaponTransform);
		shader.setUniform("view", weaponRotation);
		shader.setUniform("projection", glm::mat4(1.0f));
		renderer.renderWeapon(shader, currentWeapon);

		// render the crosshair
		crosshair.render(shader);

		window.swapBuffers();
	}

	return 0;
}

