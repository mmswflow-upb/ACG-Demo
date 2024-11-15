#include "Scene.h"
#include "dependente/glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include <iostream>

Scene::Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList)
    : backgroundImagePath(backgroundPath), enemies(enemyList)
{
    // Initialize boundaries
    for (int i = 0; i < 4; ++i) {
        boundaries[i] = boundaryArray[i];
    }

    // Initialize neighbors to nullptr
    for (int i = 0; i < 4; ++i) {
        neighbors[i] = nullptr;
    }

    backgroundTextureID = loadBackgroundTexture(backgroundPath.c_str());
    setupBackground();
}

// Function to set neighboring scenes
void Scene::setNeighbors(Scene* north, Scene* east, Scene* south, Scene* west) {
    neighbors[0] = north;
    neighbors[1] = east;
    neighbors[2] = south;
    neighbors[3] = west;
}

//Method to set the points at which the player is teleported once they transition from one scene to another
void Scene::setStartingPoints(float* west, float* east, float* south, float* north) {

    startingPoints[0] = west;
    startingPoints[1] = east;
    startingPoints[2] = south;
    startingPoints[3] = north;

}

void Scene::render(GLuint texturesProgramID, float time, Character& mainCharacter, float deltaTime) {


    glUseProgram(texturesProgramID);
    glUniform1i(glGetUniformLocation(texturesProgramID, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    glUniformMatrix4fv(glGetUniformLocation(texturesProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(backgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render each enemy in the scene
    for (Character& enemy : enemies) {
        enemy.enemyMovement(time, mainCharacter.getPosition(), deltaTime);
        enemy.render(texturesProgramID, time);  // Render each character with the character shader program
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

    unsigned int backgroundIndices[] = { 0, 3, 1, 1, 3, 2 };

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

Scene::~Scene() {
    // Delete buffers and textures for background
    glDeleteBuffers(1, &backgroundVBO);
    glDeleteBuffers(1, &backgroundIBO);
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteTextures(1, &backgroundTextureID);
}

Scene* Scene::constrainToBoundaries(Character& character) {
    glm::vec3 position = character.getPosition(); // Get character's current position

    // Check if the character is outside the left boundary (going to the west scene)
    if (position.x < boundaries[0]) {
        if (neighbors[0]) { // Check if west neighbor exists
            position.x = *neighbors[0]->startingPoints[0]; // Set to the right edge of the western scene
            character.setPosition(position);
            return neighbors[0];
        }
        position.x = boundaries[0]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the right boundary (going to the east scene)
    else if (position.x > boundaries[1]) {
        if (neighbors[1]) { // Check if east neighbor exists
            position.x = *neighbors[1]->startingPoints[1]; // Set to the left edge of the eastern scene
            character.setPosition(position);
            return neighbors[1];
        }
        position.x = boundaries[1]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the bottom boundary (going to the south scene)
    else if (position.y < boundaries[2]) {
        if (neighbors[2]) { // Check if south neighbor exists
            position.y = *neighbors[2]->startingPoints[2]; // Set to the top edge of the southern scene
            character.setPosition(position);

            return neighbors[2];
        }
        position.y = boundaries[2]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the top boundary (going to the north scene)
    else if (position.y > boundaries[3]) {
        if (neighbors[3]) { // Check if north neighbor exists
            position.y = *neighbors[3]->startingPoints[3]; // Set to the bottom edge of the northern scene
            character.setPosition(position);
            return neighbors[3];
        }
        position.y = boundaries[3]; // Constrain within current scene if no neighbor
    }

    // Update the character's position to be within the boundaries if no transition occurs
    character.setPosition(position);
    return this; // Return current scene if no boundary was crossed
}



GLuint Scene::loadBackgroundTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture with stb_image
    stbi_set_flip_vertically_on_load(true);
    int texWidth, texHeight, nrChannels;
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