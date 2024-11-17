#pragma once

#ifndef PROJECTILETYPE_H
#define PROJECTILETYPE_H

#include <string>
#include "../dependente/glm/glm.hpp"

// Enum for projectile types
enum class ProjectileType {
    Fireball,
    Fireblast,
    Iceshard,
    Bullet
};

// Struct to hold projectile attributes
struct ProjectileAttributes {
    glm::vec4 color;  // Color instead of texture path
    int damage;
};

// Function to get the attributes for each projectile type
inline ProjectileAttributes getProjectileAttributes(ProjectileType type) {
    switch (type) {
    case ProjectileType::Fireball:
        return { glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 25 }; // Orange for Fireball
    case ProjectileType::Fireblast:
        return { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 15 }; // Red for Fireblast
    case ProjectileType::Iceshard:
        return { glm::vec4(0.0f, 0.5f, 1.0f, 1.0f), 35 }; // Blue for Iceshard
    case ProjectileType::Bullet:
        return { glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 50 }; // Yellow for Bullet
    default:
        return { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0 };   // Default color white
    }
}

#endif // PROJECTILETYPE_H
