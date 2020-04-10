#ifndef ACORN_FRAMEBUFFER_H
#define ACORN_FRAMEBUFFER_H

#include "types.h"
#include <GL/gl3w.h>

struct Framebuffer {
    u32 id = 0;
    u32 width = 0;
    u32 height = 0;
    u32 depth_renderbuffer = 0;
};

Framebuffer framebuffer_create(u32 width, u32 height);

void framebuffer_destroy(Framebuffer *fbo);

void framebuffer_bind(Framebuffer *fbo);

void framebuffer_blit_to_default_framebuffer(Framebuffer *fbo, u32 mask, u32 filter);

#endif //ACORN_FRAMEBUFFER_H
