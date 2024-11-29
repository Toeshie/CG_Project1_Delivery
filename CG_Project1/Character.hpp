#pragma once
#include <glm/glm.hpp>

class Renderer; // Forward declaration

class Character {
public:
    enum class Team { CT, T };
    
    Character(const glm::vec3& pos, Team t, float rotOffset = 0.0f);
    
    glm::mat4 getModelMatrix() const;
    
    const glm::vec3& getPosition() const { return position; }
    Team getTeam() const { return team; }

    // Add friend declaration to allow Renderer to access private members
    friend class Renderer;

private:
    glm::vec3 position;
    Team team;
    float rotation;
};