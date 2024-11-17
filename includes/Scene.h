#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include "Character.h"
#include "../dependente/glm/glm.hpp"
#include "../dependente/glew/glew.h"
#include "Projectile.h"
#include "Healthbar.h"

class Scene {
public:
    std::vector<Projectile*> projectiles; //Stores the projectiles currently exisiting inside the scene, they must all be rendered


    Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList);
    ~Scene();

    //Setup VAO, IBO, VBO
    void setupBackground();
    GLuint loadBackgroundTexture(const char* filePath); //Load textures from assets folder

    //renders scene, projectiles, enemies, finds collisions between characters and projectiles then subtracts health 
    void render(GLuint texturesProgramID, GLuint projectilesProgramID, float time, Character &mainCharacter, float deltaTime, HealthBar* healthBar);

    //Dont let character go out of bounds in scenes
    Scene* constrainToBoundaries(Character& character);

    //Method to set neighboring scenes
    void setNeighbors(Scene* west, Scene* east, Scene* south, Scene* north);

    //Method to set the points at which the player is teleported once they transition from one scene to another
    void setStartingPoints(float* west, float* east, float* south, float* north);



private:
    float boundaries[4];
    float* startingPoints[4]; // [west, east, south, north] (where player teleports on each scene transition based on which direction they come from)
    std::vector<Character> enemies; //enemies that have to be rendered while they're alive and while the scene they're currently in is being rendered
    std::string backgroundImagePath;

    // Background rendering components
    GLuint backgroundVAO;
    GLuint backgroundVBO;
    GLuint backgroundIBO;
    GLuint backgroundTextureID;

    // Array of neighboring scenes
    Scene* neighbors[4]; // [west, east, south, north] (to which scene can the player transition, based on directions) 
};

#endif
