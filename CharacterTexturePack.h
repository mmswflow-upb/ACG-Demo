#pragma once

#ifndef CHARACTERTEXTUREPACK_H
#define CHARACTERTEXTUREPACK_H

#include <string>
#include <vector>

class CharacterTexturePack {
public:
    std::string idleTexturePath;
    std::vector<std::string> walkingTexturesPaths;
    std::string gunHoldingTexturePath;

    // Constructor
    CharacterTexturePack();
    CharacterTexturePack(const std::string& idlePath,
        const std::vector<std::string>& walkingPaths,
        const std::string& gunHoldingPath);
};

#endif // CHARACTERTEXTUREPACK_H
