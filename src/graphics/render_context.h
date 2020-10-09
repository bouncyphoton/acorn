#ifndef ACORN_RENDER_CONTEXT_H
#define ACORN_RENDER_CONTEXT_H

#include "texture.h"
#include "framebuffer.h"
#include <bitset>

/// A struct with all render state options
struct RenderState {
    bool depthTestEnabled = false;
};

/// A utility class for building the render state
class RenderStateBuilder {
public:
    /// Set the depth test state
    RenderStateBuilder &setDepthTest(bool enabled) {
        m_renderState.depthTestEnabled = enabled;
        return *this;
    }

    /// Finalize the built render state
    RenderState build() {
        return m_renderState;
    }

private:
    RenderState m_renderState;
};

class RenderContext {
public:
    enum ClearFlags : u32 {
        CLEAR_COLOR   = (1u << 0u),
        CLEAR_DEPTH   = (1u << 1u),
        CLEAR_STENCIL = (1u << 2u),
    };

    void setRenderTarget(const Texture2D &color);

    void clear(u32 clear_flags);

    void setState(const RenderState &state);

private:
    Framebuffer m_targetFramebuffer;
};

#endif //ACORN_RENDER_CONTEXT_H
