#pragma once

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>
#include "../dependente/glm/glm.hpp"
#include "../dependente/glew/glew.h"
#include "ProjectileType.h" // Include the ProjectileType header

// Projectile class to handle individual projectiles
class Projectile {
public:
    int damage;
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 direction;
    glm::vec4 color;  // Color for the projectile
    ProjectileType projectileType;
    float rotationAngle;

    // Constructor
    Projectile(ProjectileType type, glm::vec3 position, glm::vec3 target);

    // Destructor
    ~Projectile();

    // Renders the projectile
    void render(GLuint projectilesProgramID);

    // Updates the position of the projectile
    bool updatePosition(float deltaTime);

private:
    GLuint VAO, VBO, IBO;  // Vertex Array and Buffers for the projectile

    // Sets up vertex data for the projectile
    void setupProjectile();
};

#endif // PROJECTILE_H
