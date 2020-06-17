#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "renderable.h"
#include "game_state.h"
#include "framebuffer.h"
#include "constants.h"

struct RenderStats {
    u32 vertices_rendered = 0;
    u32 draw_calls = 0;
};

class Renderer {
public:
    void init();

    void destroy();

    void queueRenderable(Renderable renderable);

    void render();

    RenderStats getStats();

private:
    // renderable queuing
    u32 num_renderables_queued = 0;
    Renderable renderables[consts::MAX_RENDERABLES] = {};

    // textures
    u32 m_defaultFboTexture = 0;
    u32 m_environmentMap = 0;
    u32 m_diffuseIrradianceCubemap = 0;
    u32 m_prefilteredEnvCubemap = 0;
    u32 m_numPrefilteredEnvMipmapLevels; // not a texture, but important for prefiltered env cubemap
    u32 m_brdfLut = 0;

    // framebuffers
    Framebuffer m_defaultFbo = {};
    Framebuffer m_workingFbo = {};

    // shaders
    u32 m_materialShader = 0;
    u32 m_skyShader = 0;
    u32 m_diffuseIrradianceShader = 0;
    u32 m_envMapPrefilterShader = 0;
    u32 m_brdfLutShader = 0;

    // dummy vao
    u32 m_dummyVao = 0;

    // stats per frame
    RenderStats m_renderStats;
};

#endif //ACORN_RENDERER_H
