#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "renderable.h"
#include "game_state.h"
#include "framebuffer.h"
#include "constants.h"
#include "texture.h"
#include "shader.h"

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
    Texture m_defaultFboTexture;
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

    // dummy vao
    u32 m_dummyVao = 0;

    // stats per frame
    RenderStats m_renderStats;
};

#endif //ACORN_RENDERER_H
