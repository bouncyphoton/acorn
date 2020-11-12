#include "config.h"
#include "core.h"

Config::Config() {
    // TODO: load this from a file
    core->gameState.renderOptions.width = 1280;
    core->gameState.renderOptions.height = 720;
}
