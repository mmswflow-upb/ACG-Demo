#pragma once

#ifndef CHARACTERTEXTUREPACK_H
#define CHARACTERTEXTUREPACK_H

#include <string>
#include <vector>

class CharacterTexturePack {
public:
    std::string idleTexturePath; //The pic rendered when character is idle
    std::vector<std::string> walkingTexturesPaths; //The pictures used for movement animations
    std::string gunHoldingTexturePath; //The pic used when characters are holding a gun
     
    // Constructor
    CharacterTexturePack();
    CharacterTexturePack(const std::string& idlePath,
        const std::vector<std::string>& walkingPaths,
        const std::string& gunHoldingPath);
};

#endif // CHARACTERTEXTUREPACK_H
