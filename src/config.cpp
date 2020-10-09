#include "config.h"
#include "core.h"

Config::Config() {
    // TODO: load this from a file
    core->gameState.renderOptions.width = 1280;
    core->gameState.renderOptions.height = 720;
    core->gameState.scene.sunDirection = glm::normalize(glm::vec3(-1, 1, 1));
}
