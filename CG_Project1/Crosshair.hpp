#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include "ShaderProgram.hpp"

class Crosshair {
public:
    Crosshair(float windowWidth, float windowHeight);
    ~Crosshair();

    void render(const ShaderProgram& shaderProgram);

private:
    void setupBuffers();

    GLuint VAO, VBO;
    float windowWidth;
    float windowHeight;
    glm::vec3 color;
};

