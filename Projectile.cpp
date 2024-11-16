#include "Projectile.h"
#include "dependente/glm/gtc/matrix_transform.hpp"
#include "dependente/glm/gtc/type_ptr.hpp"
#include <iostream>

Projectile::Projectile(ProjectileType type, glm::vec3 position, glm::vec3 target)
    : position(position), target(target) {

    // Get projectile attributes based on type
    ProjectileAttributes attributes = getProjectileAttributes(type);
    this->damage = attributes.damage;
    this->color = attributes.color;
    this->projectileType = type;

    setupProjectile();
    direction = glm::normalize(this->target - this->position);
    rotationAngle = glm::degrees(atan2(this->direction.y, this->direction.x));
}

void Projectile::render(GLuint projectilesProgramID) {

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);  // Move to position first
    transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate around z-axis
    transform = glm::scale(transform, glm::vec3(1.0f, 0.08f, 1.0f));
    glUseProgram(projectilesProgramID);

    // Set the color for the projectile
    GLint colorLocation = glGetUniformLocation(projectilesProgramID, "projectileColor");
    if (colorLocation == -1) {
        std::cerr << "Warning: 'projectileColor' uniform not found in shader program." << std::endl;
    }
    glUniform4fv(colorLocation, 1, glm::value_ptr(color));

    // Apply transformation
    glUniformMatrix4fv(glGetUniformLocation(projectilesProgramID, "proj_transform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Draw the projectile

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool Projectile::updatePosition(float deltaTime) {
    float speed = 2.0f;  // Speed of projectile
    position += direction * speed * deltaTime;


    //// Check if the projectile went out of bounds
    if (position.x < -1.0f || position.x > 1.0f ||
        position.y < -1.0f || position.y > 1.0f) {
        return true;
    }
    return false;
}

void Projectile::setupProjectile() {
    float projectileVertices[] = {
        0.05f,  0.05f, 0.0f,
        0.05f, -0.05f, 0.0f,
       -0.05f, -0.05f, 0.0f,
       -0.05f,  0.05f, 0.0f
    };

    unsigned int projectileIndices[] = { 0, 1, 2, 2, 3, 0 };



    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(projectileVertices), projectileVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(projectileIndices), projectileIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Projectile::~Projectile() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
}
