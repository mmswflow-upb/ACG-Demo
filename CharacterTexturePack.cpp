#include "CharacterTexturePack.h"

// Constructor definition
CharacterTexturePack::CharacterTexturePack() {}

CharacterTexturePack::CharacterTexturePack(const std::string& idlePath,
    const std::vector<std::string>& walkingPaths,
    const std::string& gunHoldingPath)
    : idleTexturePath(idlePath),
    walkingTexturesPaths(walkingPaths),
    gunHoldingTexturePath(gunHoldingPath) {
}
