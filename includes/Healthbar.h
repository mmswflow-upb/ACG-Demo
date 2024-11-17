#pragma once
#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include "../dependente/glm/glm.hpp"
#include "../dependente/glew/glew.h"

class HealthBar {
public:
    // Constructor for initializing the health bar with position, sizes, and colors
    HealthBar(glm::vec3 position, glm::vec2 size, glm::vec4 backgroundColor, glm::vec4 foregroundColor);

    // Destructor for cleaning up buffers
    ~HealthBar();

    // Updates the health bar's foreground based on the current health ratio (0.0 to 1.0)
    void update(float healthRatio);

    // Renders the health bar
    void render(GLuint healthbarProgramID);

private:
    glm::vec3 position;       // Position of the health bar
    glm::vec2 size;           // Size of the health bar
    glm::vec4 backgroundColor; // Color for the background
    glm::vec4 foregroundColor; // Color for the foreground

    // OpenGL Buffers and Arrays for background and foreground
    GLuint backgroundVAO, backgroundVBO, backgroundIBO;
    GLuint foregroundVAO, foregroundVBO, foregroundIBO;

    // Setup the background and foreground vertices and buffers
    void setupBackground();
    void setupForeground();
};

#endif // HEALTHBAR_H
