#include "render_context.h"

void RenderContext::setRenderTarget(const Texture2D &color) {
    m_targetFramebuffer.bind();
    m_targetFramebuffer.attachTexture(color);
    m_targetFramebuffer.setViewport();
}

void RenderContext::clear(u32 clear_flags) {
    GLbitfield bitfield = 0;
    if (clear_flags & ClearFlags::CLEAR_COLOR) {
        bitfield |= GL_COLOR_BUFFER_BIT;
    }
    if (clear_flags & ClearFlags::CLEAR_DEPTH) {
        bitfield |= GL_DEPTH_BUFFER_BIT;
    }
    if (clear_flags & ClearFlags::CLEAR_STENCIL) {
        bitfield |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(bitfield);
}

void RenderContext::setState(const RenderState &state) {
    if (state.depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
