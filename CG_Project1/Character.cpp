#include "Character.hpp"
#include <glm/gtc/matrix_transform.hpp>

Character::Character(const glm::vec3& pos, Team t, float rotOffset) 
    : position(pos), team(t) {
    // Set initial rotation based on team plus offset
    rotation = (team == Team::CT ? 90.0f : -90.0f) + rotOffset;
}

glm::mat4 Character::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.025f)); 
    return model;
}