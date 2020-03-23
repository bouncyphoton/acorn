#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "graphics_util.h"

bool renderer_init();

void renderer_shutdown();

void renderer_queue_renderable(Renderable renderable);

void renderer_draw();

#endif //ACORN_RENDERER_H
