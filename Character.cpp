#include "Character.h"
#include "dependente/glm/gtc/matrix_transform.hpp"
#include "dependente/glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "ProjectileType.h" // For getProjectileAttributes
#include <iostream>

Character::Character(
    CharacterType characterType,
    glm::vec3 position,
    glm::vec3 scaling,
    float lookAngle,
    float startingTime,
    bool walking
) : characterType(characterType), position(position), scaling(scaling), lookAngle(lookAngle), startingTime(startingTime), walking(walking), lastShotTime(startingTime) {
    // Load character attributes based on type
    CharacterAttributes attributes = getEnemyAttributes(characterType);
    health = attributes.maxHealth;
    damage = getProjectileAttributes(attributes.projectileType).damage;
    projectileType = attributes.projectileType;
    texturePack = attributes.texturePack;
    fireRate = attributes.fireRate;
    movementAxis = attributes.movementAxis;
    lastShotTime = 0.0f;
    // Load textures once
    idleTextureID = loadTexture(texturePack.idleTexturePath.c_str());
    gunHoldingTextureID = loadTexture(texturePack.gunHoldingTexturePath.c_str());
    for (const auto& path : texturePack.walkingTexturesPaths) {
        walkingTextureIDs.push_back(loadTexture(path.c_str()));
    }

    setupCharacter();
}

void Character::render(GLuint programID, float time, float deltaTime) {


    GLuint currentTextureID;

    // Alternate between walking textures if moving
    if (walking && !walkingTextureIDs.empty()) {
        int textureIndex = static_cast<int>(time * 5) % walkingTextureIDs.size();
        currentTextureID = walkingTextureIDs[textureIndex];
    }
    else {
        currentTextureID = idleTextureID;
    }
    // Accumulate deltaTime to determine when to shoot
    lastShotTime += deltaTime;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, lookAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scaling);


    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTextureID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Character::updatePosition(float x_offset, float y_offset) {
    position.x += x_offset;
    position.y += y_offset;
}

void Character::enemyMovement(float time, glm::vec3& target, std::vector<Projectile*>& projectilesInScene, float deltaTime) {
    float movementRange = 0.8f;
    float speed = 0.5f;

    // Move between -0.8 and 0.8 on x or y axis
    if (movementAxis == 'x') {
        position.x = movementRange * sin(speed * time);
    }
    else {
        position.y = movementRange * sin(speed * time);
    }

    

    // Shoot when timeSinceLastShot meets or exceeds the fireRate
    if (lastShotTime >= fireRate) {
        shoot(target, projectilesInScene);
        lastShotTime = 0.0f;  // Reset the timer after shooting
    }
}


void Character::shoot(glm::vec3 target, std::vector<Projectile*>& projectilesInScene) {
  

    Projectile* newProjectile = new Projectile(projectileType, position, target);
    

    projectilesInScene.push_back(newProjectile);

}

void Character::setupCharacter() {
    float characterVertices[] = {
        0.25f,  0.45f, 0.0f,  1.0f, 1.0f,
        0.25f, -0.45f, 0.0f,  1.0f, 0.0f,
       -0.25f, -0.45f, 0.0f,  0.0f, 0.0f,
       -0.25f,  0.45f, 0.0f,  0.0f, 1.0f
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

GLuint Character::loadTexture(const char* filePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

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

    return texture;
}

Character::~Character() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);

    // Delete preloaded textures
    glDeleteTextures(1, &idleTextureID);
    glDeleteTextures(1, &gunHoldingTextureID);
    for (auto& texID : walkingTextureIDs) {
        glDeleteTextures(1, &texID);
    }
}

glm::vec3 Character::getPosition() const {
    return position;
}

void Character::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}
