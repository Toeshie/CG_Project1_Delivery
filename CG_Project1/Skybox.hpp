#pragma once
#include <vector>
#include <string>
#include "ShaderProgram.hpp"

class Skybox {
public:
    Skybox();
    ~Skybox();
    bool initialize(const std::vector<std::string>& faces);
    void render(const ShaderProgram& shader);

private:
    unsigned int textureID;
    unsigned int VAO, VBO;
};