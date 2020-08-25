#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "game_state.h"
#include "framebuffer.h"
#include "constants.h"
#include "texture.h"
#include "shader.h"

struct RenderStats {
    u32 verticesRendered = 0;
    u32 drawCalls = 0;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    /// Recreate entire renderer
    void recreate();

    /// Render scene to default framebuffer
    void render();

    /// Get stats on most recent frame
    RenderStats getStats();

private:
    /// Create textures, framebuffers, etc.
    void init();

    /// Cleanup textures, framebuffers, etc.
    void destroy();

    /// Run pre-compute render passes
    void precompute();

    // textures
    Texture m_defaultFboTexture;
    Texture m_workingTexture;
    Texture m_environmentMap;
    Texture m_diffuseIrradianceCubemap;
    Texture m_prefilteredEnvCubemap;
    Texture m_brdfLut;

    u32 m_numPrefilteredEnvMipmapLevels;

    // framebuffers
    Framebuffer m_defaultFbo;
    Framebuffer m_workingFbo;

    // shaders
    Shader m_materialShader;
    Shader m_skyShader;
    Shader m_diffuseIrradianceShader;
    Shader m_envMapPrefilterShader;
    Shader m_brdfLutShader;
    Shader m_tonemapShader;

    // dummy vao
    u32 m_dummyVao = 0;

    // stats per frame
    RenderStats m_renderStats;
};

#endif //ACORN_RENDERER_H
