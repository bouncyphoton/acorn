#ifndef ACORN_GAME_STATE_H
#define ACORN_GAME_STATE_H

#include "types.h"
#include "scene/scene.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct RenderOptions {
    u32 width = 800;
    u32 height = 600;
    u32 vsyncNumSwapFrames = 0;
};

struct GameState {
    RenderOptions renderOptions = {};
    Scene scene;
};

#endif //ACORN_GAME_STATE_H
