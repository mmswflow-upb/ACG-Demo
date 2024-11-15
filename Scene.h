#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include "Character.h"
#include "dependente/glew/glew.h"
#include "dependente/glm/glm.hpp"

class Scene {
public:
    Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList);

    GLuint loadBackgroundTexture(const char* filePath);
    void render(GLuint programID);
    void setupBackground();
    void constrainToBoundaries(Character& character);
    void cleanup();

private:
    std::string backgroundImagePath;
    float boundaries[4];
    std::vector<Character> enemies;

    // Background rendering components
    GLuint backgroundVAO;
    GLuint backgroundVBO;
    GLuint backgroundIBO;
    GLuint backgroundTextureID;
};

#endif
