#include "framebuffer.h"
#include "texture.h"
#include <cstdio>

Framebuffer framebuffer_create(GLenum texture_format, u32 width, u32 height) {
    Framebuffer fbo = {};
    fbo.width = width;
    fbo.height = height;

    s32 previously_bound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previously_bound);

    // generate
    glGenFramebuffers(1, &fbo.id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);

    // set texture
    fbo.texture = texture_create(texture_format, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture, 0);

    // depth renderbuffer
    glGenRenderbuffers(1, &fbo.depth_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo.depth_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo.depth_renderbuffer);

    // attach draw buffers
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "[error] framebuffer is incomplete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, previously_bound);
    return fbo;
}

void framebuffer_destroy(Framebuffer *fbo) {
    texture_destroy(fbo->texture);
    glDeleteRenderbuffers(1, &fbo->depth_renderbuffer);
    glDeleteFramebuffers(1, &fbo->id);
}

void framebuffer_bind(Framebuffer *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
    glViewport(0, 0, fbo->width, fbo->height);
}

void framebuffer_blit_to_default_framebuffer(Framebuffer *fbo, u32 mask, u32 filter) {
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fbo->width, fbo->height, dims[0], dims[1], dims[2], dims[3], mask, filter);
}
