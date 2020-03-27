#ifndef ACORN_GAME_STATE_H
#define ACORN_GAME_STATE_H

#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct RenderOptions {
    u32 width = 800;
    u32 height = 600;
    u32 vsync_num_swap_frames = 0;
};

struct Camera {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 look_at = glm::vec3(0, 0, -1);
    f32 fov_radians = glm::half_pi<f32>();
};

struct GameState {
    RenderOptions render_options = {};
    Camera camera = {};

    // unit vector pointing towards the sun
    glm::vec3 sun_direction = glm::vec3(0, 1, 0);
};

#endif //ACORN_GAME_STATE_H
