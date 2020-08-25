#ifndef ACORN_FRAMEBUFFER_H
#define ACORN_FRAMEBUFFER_H

#include "types.h"
#include <GL/gl3w.h>

// TODO: framebuffer resize

// TODO: modernize framebuffer
class Framebuffer {
public:
    void init(u32 width, u32 height);

    void destroy();

    void bind();

    void blit(Framebuffer &fbo, u32 mask, u32 filter);

    void blitToDefaultFramebuffer(u32 mask, u32 filter);

    u32 id = 0;

private:
    u32 m_width = 0;
    u32 m_height = 0;
    u32 m_depthRenderbuffer = 0;
};

#endif //ACORN_FRAMEBUFFER_H
