#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "graphics_util.h"

bool renderer_init();

void renderer_shutdown();

void renderer_draw(Renderable *renderables, u32 num_renderables);

#endif //ACORN_RENDERER_H
