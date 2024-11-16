#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include "Character.h"
#include "dependente/glew/glew.h"
#include "dependente/glm/glm.hpp"
#include "Projectile.h"
#include "Healthbar.h"

class Scene {
public:
    Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList);
    ~Scene();
    GLuint loadBackgroundTexture(const char* filePath);
    void render(GLuint texturesProgramID, GLuint projectilesProgramID, float time, Character &mainCharacter, float deltaTime, HealthBar* healthBar);
    void setupBackground();
    Scene* constrainToBoundaries(Character& character);

    //Method to set neighboring scenes
    void setNeighbors(Scene* west, Scene* east, Scene* south, Scene* north);

    //Method to set the points at which the player is teleported once they transition from one scene to another
    void setStartingPoints(float* west, float* east, float* south, float* north);

    std::vector<Projectile*> projectiles;


private:
    float boundaries[4];
    float* startingPoints[4]; // [west, east, south, north]
    std::vector<Character> enemies;
    std::string backgroundImagePath;

    // Background rendering components
    GLuint backgroundVAO;
    GLuint backgroundVBO;
    GLuint backgroundIBO;
    GLuint backgroundTextureID;

    // Array of neighboring scenes
    Scene* neighbors[4]; // [west, east, south, north]
};

#endif
