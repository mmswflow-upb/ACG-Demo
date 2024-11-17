#pragma once
#ifndef ENEMYTYPE_H
#define ENEMYTYPE_H

#include <string>
#include <vector>
#include "ProjectileType.h"
#include "CharacterTexturePack.h"

// Enum for different character types
enum class CharacterType {
    IceBoss,
    EarthBoss,
    LavaBoss,
    Main
};

// Struct to store character attributes, including texture pack for animations
struct CharacterAttributes{
    int maxHealth;
    float fireRate;                   // Fire rate in seconds (e.g., time between shots)
    CharacterTexturePack texturePack; // Texture pack for character animations
    ProjectileType projectileType;
    char movementAxis;
};

// Function to get attributes for each character type
inline CharacterAttributes getEnemyAttributes(CharacterType type) {
    switch (type) {
    case CharacterType::IceBoss:
        return { 500, 2.5f, CharacterTexturePack("assets/enemies/IceBoss.png", {}, ""), ProjectileType::Iceshard , 'y'};

    case CharacterType::EarthBoss:
        return { 400, 2.0f, CharacterTexturePack("assets/enemies/EarthBoss.png", {}, ""), ProjectileType::Fireball , 'y'};

    case CharacterType::LavaBoss:
        return { 350, 1.5f, CharacterTexturePack("assets/enemies/LavaBoss.png", {}, ""), ProjectileType::Fireblast, 'x'};

    case CharacterType::Main:
        return {
            100,
            1.0f,
            CharacterTexturePack(
                "assets/main_character/MainHoldingGun.png",  // Idle texture
                { "assets/main_character/MainWalkFirst.png", "assets/main_character/MainWalkSecond.png" },  // Walking textures
                "assets/main_character/MainHoldingGun.png"   // Gun-holding texture
            ),
            ProjectileType::Bullet,
            'a'
        };

    default:
        return { 0, 0.0f, CharacterTexturePack("", {}, ""), ProjectileType::Fireball }; // Default case with empty values
    }
}

#endif // ENEMYTYPE_H
