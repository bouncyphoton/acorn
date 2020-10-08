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

struct GraphicsDebugLogger {
    GraphicsDebugLogger();
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    /// Render scene to default framebuffer
    void render();

    /// Reload all shaders
    void reloadShaders();

    /// Get stats on most recent frame
    RenderStats getStats();

private:
    /// Setup textures, framebuffers, etc.
    void init();

    void drawNVertices(u32 n) const;

    void precompute();
    void updateIblProbe();
    void renderFrame();

    GraphicsDebugLogger m_debugLogger;

    // common
    Framebuffer m_targetFbo;
    Texture2D m_targetTexture;
    Texture2D m_hdrFrameTexture;

    Shader m_tonemapShader;

    // environment probe
    Shader m_skyShader;
    Shader m_diffuseIrradianceShader;
    Shader m_envMapPrefilterShader;
    TextureCubemap m_environmentMap;
    TextureCubemap m_diffuseIrradianceCubemap;
    TextureCubemap m_prefilteredEnvCubemap;
    u32 m_numPrefilteredEnvMipmapLevels;

    // materials
    Shader m_materialShader;
    Shader m_brdfLutShader;
    Texture2D m_brdfLut;

    // dummy vao
    u32 m_dummyVao = 0;

    // stats per frame
    RenderStats m_renderStats;
};

#endif //ACORN_RENDERER_H
