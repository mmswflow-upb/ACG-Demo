#pragma once

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include "../dependente/glm/glm.hpp"
#include "../dependente/glew/glew.h"
#include "CharacterTexturePack.h"
#include "Projectile.h"
#include "CharacterType.h" // For CharacterType and EnemyAttributes

class Character {
public:
    CharacterType characterType;

    glm::vec3 position; //current position of character
    glm::vec3 scaling; //by how much we scale the character's rectangle
    float lookAngle; //The orientation of the character (in degrees)

    float lastShotTime; //Time elapsed since the character's last shot
    float lastWalkingAnimationTime; // Time elapsed since the character's legs have "moved" (alternated between pics)
    float fireRate; //Amount of delay between each shot

    bool walking; //Whether character is walking or not
    char movementAxis; //For enemies (x axis for lava boss so right and left) (y axis for ice and earth boss so up and down)

    int health; //Current health of character
    int damage; //Damage that character deals with each shot

    ProjectileType projectileType; //The type of projectile this character shoots
    CharacterTexturePack texturePack; //The texturepack of the character (set of images used to render it)

    GLuint idleTextureID; //The pic rendered when character is idle
    std::vector<GLuint> walkingTextureIDs; //The pictures used for movement animations
    GLuint gunHoldingTextureID; //The pic used when characters are holding a gun

    GLuint VAO, VBO, IBO;// OpenGL buffers for drawing the rectangle on which the texture of the character is rendered

    Character(CharacterType characterType, glm::vec3 position, glm::vec3 scaling, float lookAngle, bool walking);
    ~Character();

    //Render the character
    void render(GLuint programID, float deltaTime);

    //Update position after user input
    void updatePosition(float x_offset, float y_offset);
    
    //Describes how enemies move, and it calls the shoot function when enough time has passed based on the fire rate
    void enemyMovement(float time,glm::vec3 &target, std::vector<Projectile*>& projectilesInScene, float deltaTime);

    //Shooting function, this generates projectiles with a given type, target and starting position
    void shoot(glm::vec3 target, std::vector<Projectile*>& projectilesInScene);
    

private:
    void setupCharacter();
    GLuint loadTexture(const char* filePath);
};


#endif // CHARACTER_H
