#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "renderable.h"
#include "game_state.h"

struct RenderStats {
    u32 vertices_rendered = 0;
    u32 draw_calls = 0;
};

class Renderer {
public:
    void init();

    void destroy();

    void queueRenderable(Renderable renderable);

    void render();

    RenderStats getStats();
};

#endif //ACORN_RENDERER_H
