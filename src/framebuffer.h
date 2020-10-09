#ifndef ACORN_FRAMEBUFFER_H
#define ACORN_FRAMEBUFFER_H

#include "types.h"
#include "graphics/texture.h"
#include <GL/gl3w.h>

// TODO: renderbuffer

class Framebuffer {
public:
    Framebuffer();
    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer &operator=(Framebuffer &&other) noexcept;
    virtual ~Framebuffer();

    /// \param texture 2D texture to attach
    void attachTexture(const Texture2D &texture);

    /// \param texture Cubemap texture to attach
    /// \param target Texture target (ex. GL_TEXTURE_CUBE_MAP_POSITIVE_X)
    /// \param level Mipmap level
    void attachTexture(const TextureCubemap &texture, u32 target, u32 level = 0);

    void setViewport(u32 mip_level = 0);

    void bind();

    void blit(Framebuffer &fbo, u32 mask, u32 filter);

    void blitToDefaultFramebuffer(u32 mask, u32 filter) const;

private:
    void handleRenderbufferCreation();

    u32 m_id = 0;
    u32 m_depthRenderbuffer = 0;
    u32 m_width = 0;
    u32 m_height = 0;
};

#endif //ACORN_FRAMEBUFFER_H
