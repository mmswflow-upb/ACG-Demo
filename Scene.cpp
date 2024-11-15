// Scene.cpp

#include "Scene.h"
#include "dependente/glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include <iostream>

Scene::Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList)
    : backgroundImagePath(backgroundPath), enemies(enemyList)
{
    // Copy the boundaries array to the member variable
    for (int i = 0; i < 4; ++i) {
        boundaries[i] = boundaryArray[i];
    }
    backgroundTextureID = loadBackgroundTexture(backgroundPath.c_str());
    setupBackground();


}

GLuint Scene::loadBackgroundTexture(const char* filePath) {
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

void Scene::render(GLuint programID) {
    // Render the background
    //glUseProgram(programID);

    //glm::mat4 backgroundTransform = glm::mat4(1.0f); // Identity matrix for background transformation
    //glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    //glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(backgroundTransform));

    //glBindVertexArray(backgroundVAO);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);

    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(backgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render each enemy in the scene
    for (Character& enemy : enemies) {
        enemy.render(programID);  // Render each scene with the scene shader program
    }
}

void Scene::setupBackground() {
    // Vertex data for the background quad
    float backgroundVertices[] = {
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  // Top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom right
       -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom left
       -1.0f,  1.0f, 0.0f, 0.0f, 1.0f   // Top left
    };

    unsigned int backgroundIndices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glGenBuffers(1, &backgroundIBO);

    glBindVertexArray(backgroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // Unbind VAO
}

void Scene::cleanup() {
    // Delete buffers and textures for background
    glDeleteBuffers(1, &backgroundVBO);
    glDeleteBuffers(1, &backgroundIBO);
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteTextures(1, &backgroundTextureID);
}

void Scene::constrainToBoundaries(Character& scene) {
    glm::vec3 position = scene.getPosition(); // Get scene's current position

    // Check if the scene is outside the left boundary
    if (position.x < boundaries[0]) {
        position.x = boundaries[0];
    }
    // Check if the scene is outside the right boundary
    if (position.x > boundaries[1]) {
        position.x = boundaries[1];
    }
    // Check if the scene is outside the bottom boundary
    if (position.y < boundaries[2]) {
        position.y = boundaries[2];
    }
    // Check if the scene is outside the top boundary
    if (position.y > boundaries[3]) {
        position.y = boundaries[3];
    }

    // Update the scene's position to be within the boundaries
    scene.setPosition(position);
}

