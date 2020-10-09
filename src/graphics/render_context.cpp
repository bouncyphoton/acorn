#include "render_context.h"

RenderContext::RenderContext() {
    // NOTE: this isn't something that should change and
    // doesn't make sense as a render state option to expose
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void RenderContext::setRenderTarget(const Texture2D &color) {
    m_targetFramebuffer.bind();
    m_targetFramebuffer.attachTexture(color);
    m_targetFramebuffer.setViewport();
}

void RenderContext::setRenderTarget(const TextureCubemap &color, CubemapFaceEnum face, u32 mip_level) {
    m_targetFramebuffer.bind();
    m_targetFramebuffer.attachTexture(color, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (u32)face, mip_level);
    m_targetFramebuffer.setViewport(mip_level);
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

    if (state.depthWriteEnabled) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }

    glDepthFunc(GL_NEVER + (u32)state.depthFunc);
}
