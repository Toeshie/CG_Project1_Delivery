#include "Crosshair.hpp"
#include <glm/gtc/matrix_transform.hpp>

Crosshair::Crosshair(float windowWidth, float windowHeight) 
    : windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , color(1.0f, 0.0f, 0.0f)  // Red color
{
    setupBuffers();
}

Crosshair::~Crosshair() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Crosshair::setupBuffers() {
    float crosshairSize = 10.0f;  // Size in pixels
    std::vector<float> vertices = {
        // Position (x,y,z)    // Texture coords (u,v)
        // Horizontal line
        -crosshairSize, 0.0f, 0.0f,    0.0f, 0.0f,
        crosshairSize, 0.0f, 0.0f,     0.0f, 0.0f,
        // Vertical line
        0.0f, -crosshairSize, 0.0f,    0.0f, 0.0f,
        0.0f, crosshairSize, 0.0f,     0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Crosshair::render(const ShaderProgram& shaderProgram) {
    GLboolean depthTest;
    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
    
    glDisable(GL_DEPTH_TEST);
    
    shaderProgram.use();
    
    glm::mat4 ortho = glm::ortho(-windowWidth/2, windowWidth/2, -windowHeight/2, windowHeight/2);
    shaderProgram.setUniform("projection", ortho);
    shaderProgram.setUniform("view", glm::mat4(1.0f));
    shaderProgram.setUniform("model", glm::mat4(1.0f));
    shaderProgram.setUniform("isCrosshair", true);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    
    shaderProgram.setUniform("isCrosshair", false);
    
    if (depthTest) glEnable(GL_DEPTH_TEST);
}