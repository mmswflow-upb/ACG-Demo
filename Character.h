#pragma once
// Character.h

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include "dependente/glm/glm.hpp"
#include "dependente/glew/glew.h"

class Character {
public:
    // Public properties for character stats
    int health;
    int damage;
    glm::vec3 position;
    glm::vec3 scaling;
    GLuint textureID;
    GLuint VAO, VBO, IBO;

    // Constructor
    Character(const std::string& texturePath, int health, int damage, glm::vec3 position, glm::vec3 scaling);

    // Function to render the character
    void render(GLuint programID);

    // Function to update character position based on input
    void updatePosition(float x_offset, float y_offset);

    // Getter function for position
    glm::vec3 getPosition() const;

    void setPosition(const glm::vec3& newPosition);

    void cleanup();

private:
    // Private method to set up VAO, VBO, and IBO for the character
    void setupCharacter();

    // Method to load texture from file
    GLuint loadTexture(const char* filePath);
};

#endif // CHARACTER_H
