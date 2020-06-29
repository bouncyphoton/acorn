#include "framebuffer.h"
#include "core.h"

void Framebuffer::init(u32 width, u32 height) {
    m_width = width;
    m_height = height;

    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    // generate
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // attach draw buffers
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // depth renderbuffer
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        core->fatal("Framebuffer is incomplete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
}

void Framebuffer::destroy() {
    glDeleteRenderbuffers(1, &m_depthRenderbuffer);
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, m_width, m_height);
}

void Framebuffer::blitToDefaultFramebuffer(u32 mask, u32 filter) {
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_width, m_height, dims[0], dims[1], dims[2], dims[3], mask, filter);
}
