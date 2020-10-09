#ifndef ACORN_RENDER_CONTEXT_H
#define ACORN_RENDER_CONTEXT_H

#include "texture.h"
#include "framebuffer.h"
#include <bitset>

/// Enum of depth comparison functions
enum class DepthFuncEnum : u32 {
    NEVER = 0,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

/// A struct with all render state options
struct RenderState {
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    DepthFuncEnum depthFunc = DepthFuncEnum::LESS;
};

/// A utility class for building the render state
class RenderStateBuilder {
public:
    /// Set the depth test state
    RenderStateBuilder &setDepthTest(bool enabled) {
        m_renderState.depthTestEnabled = enabled;
        return *this;
    }

    /// Set the depth write state
    RenderStateBuilder &setDepthWrite(bool enabled) {
        m_renderState.depthWriteEnabled = enabled;
        return *this;
    }

    /// Set depth comparison function
    RenderStateBuilder &setDepthFunc(DepthFuncEnum func) {
        m_renderState.depthFunc = func;
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

    enum CubemapFaceEnum : u32 {
        POSITIVE_X = 0,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    RenderContext();

    void setRenderTarget(const Texture2D &color);

    void setRenderTarget(const TextureCubemap &color, CubemapFaceEnum face, u32 mip_level = 0);

    void clear(u32 clear_flags);

    void setState(const RenderState &state);

    const Framebuffer &getFramebuffer() const {
        return m_targetFramebuffer;
    }

private:
    Framebuffer m_targetFramebuffer;
};

#endif //ACORN_RENDER_CONTEXT_H
