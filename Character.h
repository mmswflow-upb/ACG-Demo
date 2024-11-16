#pragma once

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include "dependente/glm/glm.hpp"
#include "dependente/glew/glew.h"
#include "CharacterTexturePack.h"
#include "CharacterType.h" // For CharacterType and EnemyAttributes

class Character {
public:
    CharacterType characterType;
    glm::vec3 position;
    glm::vec3 scaling;
    float lookAngle;
    float startingTime;
    float lastShotTime;
    bool walking;
    char movementAxis;
    int health;
    CharacterTexturePack texturePack;

    GLuint idleTextureID;
    std::vector<GLuint> walkingTextureIDs;
    GLuint gunHoldingTextureID;

    GLuint VAO, VBO, IBO;

    Character(CharacterType characterType, glm::vec3 position, glm::vec3 scaling, float lookAngle, float startingTime, bool walking);
    ~Character();

    void render(GLuint programID, float time);
    void updatePosition(float x_offset, float y_offset);
    void enemyMovement(float time, glm::vec3& target, float deltaTime);
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& newPosition);

private:
    void setupCharacter();
    GLuint loadTexture(const char* filePath);
};


#endif // CHARACTER_H
