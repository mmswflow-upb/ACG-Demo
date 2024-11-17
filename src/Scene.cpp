#include "../includes/Scene.h"
#include "../dependente/glm/gtc/type_ptr.hpp"
#include "../includes/stb_image.h"
#include <iostream>

Scene::Scene(const std::string& backgroundPath, const float boundaryArray[4], const std::vector<Character>& enemyList)
    : backgroundImagePath(backgroundPath), enemies(enemyList), projectiles(std::vector<Projectile*>())
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

bool isProjectileCollidingWithCharacter(const glm::vec3& projectilePosition, const Character& character) {
   

    float left, right, bottom, top;

    // Set hitbox values based on character type
    switch (character.characterType) {
    case CharacterType::Main:
        left = character.position.x - 0.05f;
        right = character.position.x + 0.05f;
        bottom = character.position.y - 0.15f;
        top = character.position.y + 0.15f;
        break;

    case CharacterType::IceBoss:
        left = character.position.x - 0.45f;
        right = character.position.x + 0.45f;
        bottom = character.position.y - 0.1f;
        top = character.position.y + 0.1f;
        break;

    case CharacterType::LavaBoss:
        left = character.position.x - 0.14f;
        right = character.position.x + 0.01f;
        bottom = character.position.y - 0.35f;
        top = character.position.y + 0.45f;
        break;

    case CharacterType::EarthBoss:
        left = character.position.x - 0.45f;
        right = character.position.x + 0.4f;
        bottom = character.position.y - 0.1f;
        top = character.position.y + 0.1f;
        break;

        // Add cases for additional enemy types as needed
    default:
        // Default hitbox size if character type is unrecognized
        left = character.position.x - 0.25f;
        right = character.position.x + 0.25f;
        bottom = character.position.y - 0.45f;
        top = character.position.y + 0.45f;
        break;
    }

    // Check if projectile is within the hitbox boundaries
    return (projectilePosition.x > left && projectilePosition.x < right &&
        projectilePosition.y > bottom && projectilePosition.y < top);
}

void Scene::render(GLuint texturesProgramID, GLuint projectilesProgramID, float time,Character &mainCharacter, float deltaTime, HealthBar* healthBar) {
   

    glUseProgram(texturesProgramID);
    glUniform1i(glGetUniformLocation(texturesProgramID, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    glUniformMatrix4fv(glGetUniformLocation(texturesProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(backgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render each enemy in the scene
    for (Character& enemy : enemies) {
        //Make each enemy move and based on delta time & shoot when they reach their fire rate time
        enemy.enemyMovement(time, mainCharacter.position, projectiles, deltaTime);
        enemy.render(texturesProgramID, deltaTime);  // Render each character with the character shader program
    }

    

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        (*it)->render(projectilesProgramID);
        bool wentOutOfBounds = (*it)->updatePosition(deltaTime);

        // Handle collision with the player if the projectile is an enemy type
        if ((*it)->projectileType == ProjectileType::Fireball ||
            (*it)->projectileType == ProjectileType::Fireblast ||
            (*it)->projectileType == ProjectileType::Iceshard) {

            if (isProjectileCollidingWithCharacter((*it)->position, mainCharacter)) {


                //If player was hit by an enemy's projectile then their health is subtracted based on the type of the projectile
                //And their health cant go below zero
                if (mainCharacter.health - (*it)->damage >= 0) {
                    mainCharacter.health -= (*it)->damage;
                }
                else {
                    mainCharacter.health = 0;
                }
                std::cout << "Character health after getting shot: " << mainCharacter.health << "\n";

                healthBar->update(mainCharacter.health / 100.0f); // Update health bar

                delete* it;
                it = projectiles.erase(it);
                continue; // Move to the next projectile after handling this one
            }
        }

        // Handle collision with enemies if the projectile is the player's type
        else if ((*it)->projectileType == ProjectileType::Bullet) {
            bool enemyHit = false; // Flag to track if any enemy is hit
            for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy) {
                if (isProjectileCollidingWithCharacter((*it)->position, *enemy)) {
                    if (enemy->health - (*it)->damage >= 0) {
                        enemy->health -= (*it)->damage; //Subtract health from enemy when hit by bullets
                    }
                    else {
                        enemy->health = 0;
                        std::cout << "Enemy defeated\n";
                        enemies.erase(enemy); // Remove enemy from the scene if defeated
                    }
                   

                    delete* it;
                    it = projectiles.erase(it); // Remove projectile after it hits an enemy
                    enemyHit = true; // Set flag to true if an enemy is hit
                    break; // Exit the enemy loop after the hit to prevent double collisions
                }
            }
            if (enemyHit) continue; // Skip incrementing `it` if a projectile was erased
        }

        // If the projectile went out of bounds
        if (wentOutOfBounds) {
            std::cout << ("Projectile went out of bounds\n\n");
            delete* it;
            it = projectiles.erase(it);
        }
        else {
            ++it; // Only increment if not erased
        }
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
    glm::vec3 position = character.position; // Get character's current position

    // Check if the character is outside the left boundary (going to the west scene)
    if (position.x < boundaries[0]) {
        if (neighbors[0]) { // Check if west neighbor exists
            position.x = *neighbors[0]->startingPoints[0]; // Set to the right edge of the western scene
            character.position = position;
            return neighbors[0];
        }
        position.x = boundaries[0]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the right boundary (going to the east scene)
    else if (position.x > boundaries[1]) {
        if (neighbors[1]) { // Check if east neighbor exists
            position.x = *neighbors[1]->startingPoints[1]; // Set to the left edge of the eastern scene
            character.position = position;
            return neighbors[1];
        }
        position.x = boundaries[1]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the bottom boundary (going to the south scene)
    else if (position.y < boundaries[2]) {
        if (neighbors[2]) { // Check if south neighbor exists
            position.y = *neighbors[2]->startingPoints[2]; // Set to the top edge of the southern scene
            character.position = position;
            return neighbors[2];
        }
        position.y = boundaries[2]; // Constrain within current scene if no neighbor
    }
    // Check if the character is outside the top boundary (going to the north scene)
    else if (position.y > boundaries[3]) {
        if (neighbors[3]) { // Check if north neighbor exists
            position.y = *neighbors[3]->startingPoints[3]; // Set to the bottom edge of the northern scene
            character.position = position;
            return neighbors[3];
        }
        position.y = boundaries[3]; // Constrain within current scene if no neighbor
    }

    // Update the character's position to be within the boundaries if no transition occurs
    character.position = position;
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