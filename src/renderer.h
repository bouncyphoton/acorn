#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "renderable.h"
#include "game_state.h"

struct RenderStats {
    u32 vertices_rendered = 0;
    u32 draw_calls = 0;
};

bool renderer_init();

void renderer_shutdown();

void renderer_queue_renderable(Renderable renderable);

void renderer_draw(GameState *game_state);

RenderStats renderer_get_stats();

#endif //ACORN_RENDERER_H
