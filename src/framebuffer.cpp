#include "framebuffer.h"
#include "log.h"
#include <cmath>

Framebuffer::Framebuffer() {
    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    glGenFramebuffers(1, &m_id);
    if (m_id == 0) {
        Log::fatal("Failed to create Framebuffer");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
    Log::debug("Framebuffer::Framebuffer() - #%d", m_id);
}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : m_id(other.m_id), m_depthRenderbuffer(other.m_depthRenderbuffer),
      m_width(other.m_width), m_height(other.m_height) {
    other.m_id = 0;
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
    m_id = other.m_id;
    m_depthRenderbuffer = other.m_depthRenderbuffer;
    m_width = other.m_width;
    m_height = other.m_height;

    other.m_id = 0;
    other.m_depthRenderbuffer = 0;
    return *this;
}

Framebuffer::~Framebuffer() {
    Log::debug("Framebuffer::~Framebuffer() - #%d", m_id);
    glDeleteRenderbuffers(1, &m_depthRenderbuffer);
    glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::attachTexture(const Texture2D &texture) {
    m_width = texture.getWidth();
    m_height = texture.getHeight();

    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    bind();

    // Set texture
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.getId(), 0);

    // TODO: profile, there's probably a good amount of processing time spent here

    handleRenderbufferCreation();

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
}

void Framebuffer::attachTexture(const TextureCubemap &texture, u32 target, u32 level) {
    m_width = texture.getSideLength();
    m_height = texture.getSideLength();

    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    bind();

    // Set texture
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture.getId(), level);

    handleRenderbufferCreation();

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
}

void Framebuffer::setViewport(u32 mip_level) {
    f32 scale = mip_level == 0 ? 1 : std::pow(0.5f, mip_level);
    glViewport(0, 0, (u32)(m_width * scale), (u32)(m_height * scale));
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::blit(Framebuffer &fbo, u32 mask, u32 filter) {
    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.m_id);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, fbo.m_width, fbo.m_height, mask, filter);

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
}

void Framebuffer::blitToDefaultFramebuffer(u32 mask, u32 filter) const {
    s32 previouslyBound;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previouslyBound);

    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_width, m_height, dims[0], dims[1], dims[2], dims[3], mask, filter);

    glBindFramebuffer(GL_FRAMEBUFFER, previouslyBound);
}

void Framebuffer::handleRenderbufferCreation() {
    // Delete old renderbuffer if it exists
    glDeleteRenderbuffers(1, &m_depthRenderbuffer);
    m_depthRenderbuffer = 0;

    // Create depth renderbuffer for this texture
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    if (m_depthRenderbuffer == 0) {
        Log::fatal("Failed to generate depth renderbuffer for framebuffer #%d", m_id);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);

    // Verify framebuffer completeness
    u32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        Log::fatal("Framebuffer #%d is incomplete: %d", m_id, status);
    }
}
