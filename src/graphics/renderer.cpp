#include "renderer.h"
#include "types.h"
#include "utils.h"
#include "core.h"
#include "log.h"
#include "constants.h"
#include <stb_image.h>
#include <GL/gl3w.h>

/*
 * GOAL: it should be clear (by looking at the code here) that the renderer has these stages/render passes:
 *
 * [PRECOMPUTE]
 * render to brdfLut2d
 *
 * [UPDATE IBL PROBE]
 * load envMapCubemap from file or generate sky
 * use envMapCubemap -> render to diffuseIrradianceCubemap
 * use envMapCubemap -> render to prefilteredEnvCubemap
 *
 * [RENDER FRAME]
 * use brdfLut, diffuseIrradianceCubemap, prefilteredEnvCubemap -> render to hdrTexture
 * use envMap -> render to hdrTexture
 * use hdrTexture -> render to ldrTexture
 */

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        Log::warn("[error][opengl] %s", message);
    } else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        Log::info("[opengl] %s", message);
    }
}

GraphicsDebugLogger::GraphicsDebugLogger() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);
}

// TODO: load shaders from resource manager instead

Renderer::Renderer()
    : m_materialShader("../assets/shaders/material.vert", "../assets/shaders/material.frag"),
      m_skyShader("../assets/shaders/cube.vert", "../assets/shaders/sky.frag"),
      m_diffuseIrradianceShader("../assets/shaders/cube.vert", "../assets/shaders/diffuse_irradiance_convolution.frag"),
      m_envMapPrefilterShader("../assets/shaders/cube.vert", "../assets/shaders/env_map_prefilter.frag"),
      m_brdfLutShader("../assets/shaders/fullscreen.vert", "../assets/shaders/brdf_lut.frag"),
      m_tonemapShader("../assets/shaders/fullscreen.vert", "../assets/shaders/tonemap.frag") {
    Log::debug("Renderer::Renderer()");
    init();
}

Renderer::~Renderer() {
    Log::debug("Renderer::~Renderer()");
}

void Renderer::init() {
    //--------------
    // init textures
    //--------------

    // TODO: don't hardcode skybox textures into renderer
    stbi_set_flip_vertically_on_load(0);
    s32 w, h;
    void *data[6] = {
        stbi_loadf("../assets/env/px.hdr", &w, &h, nullptr, 3),
        stbi_loadf("../assets/env/nx.hdr", &w, &h, nullptr, 3),
        stbi_loadf("../assets/env/py.hdr", &w, &h, nullptr, 3),
        stbi_loadf("../assets/env/ny.hdr", &w, &h, nullptr, 3),
        stbi_loadf("../assets/env/pz.hdr", &w, &h, nullptr, 3),
        stbi_loadf("../assets/env/nz.hdr", &w, &h, nullptr, 3)
    };
    if (w != h) {
        Log::fatal("skybox side textures are not square");
    }
    m_environmentMap.setImage(w, TextureFormatEnum::RGB16F, data);
    for (int i = 0; i < 6; ++i) {
        stbi_image_free(data[i]);
    }
    stbi_set_flip_vertically_on_load(1);

    m_diffuseIrradianceCubemap.setImage(consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_prefilteredEnvCubemap.setImage(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_brdfLut.setImage(consts::BRDF_LUT_TEXTURE_SIZE, consts::BRDF_LUT_TEXTURE_SIZE, TextureFormatEnum::RG16F);
    m_hdrFrameTexture.setImage(core->gameState.renderOptions.width, core->gameState.renderOptions.height,
                               TextureFormatEnum::RGB16F);
    m_targetTexture.setImage(core->gameState.renderOptions.width, core->gameState.renderOptions.height,
                             TextureFormatEnum::RGBA8);

    //----------
    // dummy vao
    //----------

    glGenVertexArrays(1, &m_dummyVao);
    if (m_dummyVao == 0) {
        Log::fatal("Failed to generate dummy VAO");
    }

    precompute();
    updateIblProbe();
}

void Renderer::render(const Scene &scene) {
    // TODO: iterate over ibl probes
    // if needs to update, update

    m_ctx.setRenderTarget(m_hdrFrameTexture);
    m_ctx.clear(RenderContext::CLEAR_COLOR | RenderContext::CLEAR_DEPTH);

    // draw scene
    {
        m_renderStats = {};

        m_ctx.setState(RenderStateBuilder()
                       .setDepthTest(true)
                       .build());

        m_materialShader.bind();
        m_materialShader.setUniform("uDiffuseIrradianceMap", m_diffuseIrradianceCubemap);
        m_materialShader.setUniform("uPrefilteredEnvironmentMap", m_prefilteredEnvCubemap);
        m_materialShader.setUniform("uNumPrefilteredEnvMipmapLevels", m_numPrefilteredEnvMipmapLevels);
        m_materialShader.setUniform("uBrdfLut", m_brdfLut);
        m_materialShader.setUniform("uSunDirection", scene.getSunDirection());
        m_materialShader.setUniform("uViewProjectionMatrix", scene.getCamera().getViewProjectionMatrix());
        m_materialShader.setUniform("uCameraPosition", scene.getCamera().getPosition());

        // render entities
        for (const Geometry &geometry : scene.getGeometry()) {
            glm::mat4 modelMatrix = geometry.getTransform().toMatrix();
            m_materialShader.setUniform("uModelMatrix", modelMatrix);
            m_materialShader.setUniform("uNormalMatrix", glm::transpose(glm::inverse(modelMatrix)));

            // render each mesh in model
            for (const Mesh &mesh : geometry.getModel()->getMeshes()) {
                m_materialShader.setUniform("uMaterial.albedo", *mesh.getMaterial().albedoTexture);
                m_materialShader.setUniform("uMaterial.normal", *mesh.getMaterial().normalTexture);
                m_materialShader.setUniform("uMaterial.metallic", *mesh.getMaterial().metallicTexture);
                m_materialShader.setUniform("uMaterial.metallic_scale", mesh.getMaterial().metallicScale);
                m_materialShader.setUniform("uMaterial.roughness", *mesh.getMaterial().roughnessTexture);
                m_materialShader.setUniform("uMaterial.roughness_scale", mesh.getMaterial().roughnessScale);

                mesh.draw();

                ++m_renderStats.drawCalls;
                m_renderStats.verticesRendered += mesh.getNumVertices();
            }
        }
    }

    // draw sky
    {
        m_ctx.setState(RenderStateBuilder()
                       .setDepthTest(true)
                       .setDepthWrite(false)
                       .setDepthFunc(DepthFuncEnum::LESS_EQUAL)
                       .build());

        Camera skyboxCamera = scene.getCamera();
        skyboxCamera.setPosition(glm::vec3(0));

        m_skyShader.bind();
        m_skyShader.setUniform("uViewProjectionMatrix", skyboxCamera.getViewProjectionMatrix());
        m_skyShader.setUniform("uEnvMap", m_environmentMap);

        drawNVertices(14);
    }

    // tonemap
    {
        m_ctx.setRenderTarget(m_targetTexture);
        m_ctx.setState(RenderStateBuilder()
                       .setDepthTest(false)
                       .build());

        m_tonemapShader.bind();
        m_tonemapShader.setUniform("uImage", m_hdrFrameTexture);
        m_tonemapShader.setUniform("uExposure", scene.getCamera().getExposure());

        drawNVertices(4);
    }

    // blit rendered frame to default framebuffer
    m_ctx.getFramebuffer().blitToDefaultFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unbind shaders
    glUseProgram(0);
}

void Renderer::reloadShaders() {
    m_materialShader.reload();
    m_skyShader.reload();
    m_diffuseIrradianceShader.reload();
    m_envMapPrefilterShader.reload();
    m_brdfLutShader.reload();
    m_tonemapShader.reload();
}

RenderStats Renderer::getStats() {
    return m_renderStats;
}

void Renderer::drawNVertices(u32 n) const {
    glBindVertexArray(m_dummyVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, n);
    glBindVertexArray(0);
}

void Renderer::precompute() {
    // We want to render to the brdfLut texture
    m_ctx.setRenderTarget(m_brdfLut);

    // Set render state
    m_ctx.setState(RenderStateBuilder()
                   .setDepthTest(false)
                   .build());

    // Clear screen
    m_ctx.clear(RenderContext::CLEAR_COLOR);

    // Bind shader and draw vertices
    m_brdfLutShader.bind();
    drawNVertices(4);

    // Generate mipmap
    m_brdfLut.generateMipmap();
}

void Renderer::updateIblProbe() {
    // view and projection matrices for cubemap rendering
    glm::mat4 views[6] = {
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))
    };
    glm::mat4 proj = glm::perspective(glm::half_pi<f32>(), 1.0f, 0.01f, 10.0f);

    // TODO: convolution/prefiltering could be improved performance-wise

    //-------------------------------
    // diffuse irradiance convolution
    //-------------------------------

    m_ctx.setState(RenderStateBuilder()
                   .setDepthTest(false)
                   .build());

    m_diffuseIrradianceShader.bind();
    m_diffuseIrradianceShader.setUniform("uEnvMap", m_environmentMap);

    for (u32 face = 0; face < 6; ++face) {
        // set current face as output color attachment
        m_diffuseIrradianceShader.setUniform("uViewProjectionMatrix", proj * views[face]);
        m_ctx.setRenderTarget(m_diffuseIrradianceCubemap, (RenderContext::CubemapFaceEnum)face);
        m_ctx.clear(RenderContext::CLEAR_COLOR);

        // draw cube
        drawNVertices(14);
    }

    // update mipmap for diffuse irradiance cubemap
    m_diffuseIrradianceCubemap.generateMipmap();

    //--------------------------
    // prefilter environment map
    //--------------------------

    m_envMapPrefilterShader.bind();
    m_envMapPrefilterShader.setUniform("uEnvMap", m_environmentMap);

    // calculate mipmap levels
    m_numPrefilteredEnvMipmapLevels = floor(log2(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE));

    for (u32 level = 0; level <= m_numPrefilteredEnvMipmapLevels; ++level) {
        // set current roughness for prefilter
        f32 roughness = (f32) level / (f32) (m_numPrefilteredEnvMipmapLevels);
        m_envMapPrefilterShader.setUniform("uRoughness", roughness);

        for (u32 face = 0; face < 6; ++face) {
            // set current face as output color attachment
            m_envMapPrefilterShader.setUniform("uViewProjectionMatrix", proj * views[face]);
            m_ctx.setRenderTarget(m_prefilteredEnvCubemap, (RenderContext::CubemapFaceEnum)face, level);
            m_ctx.clear(RenderContext::CLEAR_COLOR);

            // draw cube
            drawNVertices(14);
        }
    }
}
