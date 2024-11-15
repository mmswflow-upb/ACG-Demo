// Character.cpp

#include "Character.h"
#include "dependente/glm/gtc/matrix_transform.hpp"
#include "dependente/glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include <iostream>

Character::Character(const std::string& texturePath, int health, int damage, glm::vec3 position, glm::vec3 scaling)
    : health(health), damage(damage), position(position), scaling(scaling) {
    textureID = loadTexture(texturePath.c_str());
    setupCharacter();
}

void Character::render(GLuint programID) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::scale(transform, scaling);

    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Character::updatePosition(float x_offset, float y_offset) {
    position.x += x_offset;
    position.y += y_offset;
}

void Character::setupCharacter() {
    // Vertex data for character
    float characterVertices[] = {
        0.5f,  0.25f, 0.0f,  1.0f, 1.0f,  // top right
        0.5f, -0.25f, 0.0f,  1.0f, 0.0f,  // bottom right
       -0.5f, -0.25f, 0.0f,  0.0f, 0.0f,  // bottom left
       -0.5f,  0.25f, 0.0f,  0.0f, 1.0f   // top left
    };

    unsigned int characterIndices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(characterVertices), characterVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(characterIndices), characterIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture Coords
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
}

GLuint Character::loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath, &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void Character::cleanup() {
    // Delete buffers and textures for background
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &textureID);
}

// Getter function for position
glm::vec3 Character::getPosition() const {
    return position;
}

// Setter function for position
void Character::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}
