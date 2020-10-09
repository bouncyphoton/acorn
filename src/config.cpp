#include "config.h"
#include "core.h"

Config::Config() {
    // TODO: load this from a file
    core->gameState.renderOptions.width = 1024;
    core->gameState.renderOptions.height = 768;
    core->gameState.scene.sunDirection = glm::normalize(glm::vec3(-1, 1, 1));
}
