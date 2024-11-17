#include "../includes/HealthBar.h"
#include "../dependente/glm/gtc/matrix_transform.hpp"
#include "../dependente/glm/gtc/type_ptr.hpp"

HealthBar::HealthBar(glm::vec3 position, glm::vec2 size, glm::vec4 backgroundColor, glm::vec4 foregroundColor)
    : position(position), size(size), backgroundColor(backgroundColor), foregroundColor(foregroundColor) {

    setupBackground();
    setupForeground();
}

HealthBar::~HealthBar() {
    glDeleteBuffers(1, &backgroundVBO);
    glDeleteBuffers(1, &backgroundIBO);
    glDeleteVertexArrays(1, &backgroundVAO);

    glDeleteBuffers(1, &foregroundVBO);
    glDeleteBuffers(1, &foregroundIBO);
    glDeleteVertexArrays(1, &foregroundVAO);
}

void HealthBar::setupBackground() {
    float backgroundVertices[] = {
        position.x, position.y, 0.0f,           // Top-left
        position.x, position.y - size.y, 0.0f,  // Bottom-left
        position.x + size.x, position.y - size.y, 0.0f,  // Bottom-right
        position.x + size.x, position.y, 0.0f   // Top-right
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glGenBuffers(1, &backgroundIBO);

    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void HealthBar::setupForeground() {
    //The foreground has to be the same size as background
    float foregroundVertices[] = {
        position.x, position.y, 0.0f,              // Top-left
        position.x, position.y - size.y, 0.0f, // Bottom-left
        position.x + size.x, position.y - size.y, 0.0f, // Bottom-right
        position.x + size.x, position.y, 0.0f      // Top-right
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &foregroundVAO);
    glGenBuffers(1, &foregroundVBO);
    glGenBuffers(1, &foregroundIBO);

    glBindVertexArray(foregroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, foregroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(foregroundVertices), foregroundVertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, foregroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void HealthBar::update(float healthRatio) {

    // Update the foreground width based on health ratio
    float adjustedWidth = size.x * healthRatio;

    float foregroundVertices[] = {
        position.x, position.y, 0.0f,                      // Top-left
        position.x, position.y - size.y, 0.0f,     // Bottom-left
        position.x + adjustedWidth, position.y - size.y, 0.0f, // Bottom-right
        position.x + adjustedWidth, position.y, 0.0f       // Top-right
    };

    glBindBuffer(GL_ARRAY_BUFFER, foregroundVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(foregroundVertices), foregroundVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void HealthBar::render(GLuint healthbarProgramID) {
    glUseProgram(healthbarProgramID);

    // Render the background
    GLuint colorLocation = glGetUniformLocation(healthbarProgramID, "healthbarColor");
    GLuint transformLocation = glGetUniformLocation(healthbarProgramID, "healthbar_transform");
    glUniform4fv(colorLocation, 1, glm::value_ptr(backgroundColor));
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(backgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render the foreground
    glUniform4fv(colorLocation, 1, glm::value_ptr(foregroundColor));
    glBindVertexArray(foregroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
