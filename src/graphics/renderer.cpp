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
    const ConfigData &config = core->config.getConfigData();

    //--------------
    // init textures
    //--------------

    m_brdfLut.setImage(consts::BRDF_LUT_TEXTURE_SIZE, consts::BRDF_LUT_TEXTURE_SIZE, TextureFormatEnum::RG16F);
    m_hdrFrameTexture.setImage(config.width, config.height, TextureFormatEnum::RGB16F);
    m_targetTexture.setImage(config.width, config.height, TextureFormatEnum::RGBA8);

    //----------
    // dummy vao
    //----------

    glGenVertexArrays(1, &m_dummyVao);
    if (m_dummyVao == 0) {
        Log::fatal("Failed to generate dummy VAO");
    }

    precompute();
}

void Renderer::update(Scene &scene) {
    // view and projection matrices for cubemap rendering
    glm::mat4 proj = glm::perspective(glm::half_pi<f32>(), 1.0f, 0.01f, 10.0f);
    auto getView = [](u32 face, glm::vec3 position = glm::vec3(0)) -> glm::mat4 {
        glm::mat4 views[6] = {
            glm::lookAt(position, position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
            glm::lookAt(position, position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
            glm::lookAt(position, position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            glm::lookAt(position, position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
            glm::lookAt(position, position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
            glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))
        };

        return views[face];
    };

    const IblProbe &distantProbe = scene.getDistantIblProbe();

    for (IblProbe &probe : scene.getIblProbes()) {
        // If we don't need to update, go to next one
        if (!probe.isDirty()) {
            continue;
        }

        // Capture environment
        // TODO: this would be better if we only had one draw call instead of one per face
        for (int face = 0; face < 6; ++face) {
            m_ctx.setState(RenderStateBuilder()
                           .setDepthTest(true)
                           .build());

            m_ctx.setRenderTarget(probe.getEnvironmentMap(), (RenderContext::CubemapFaceEnum)face);
            m_ctx.clear(RenderContext::CLEAR_COLOR | RenderContext::CLEAR_DEPTH);

            m_materialShader.bind();

            m_materialShader.setUniform("uDiffuseIrradianceMap", distantProbe.getDiffuseIrradianceCubemap());
            m_materialShader.setUniform("uPrefilteredEnvironmentMap", distantProbe.getPrefilteredCubemap());
            m_materialShader.setUniform("uNumPrefilteredEnvMipmapLevels", distantProbe.getNumPrefilteredMipmapLevels());

            m_materialShader.setUniform("uBrdfLut", m_brdfLut);
            m_materialShader.setUniform("uSunDirection", scene.getSunDirection());
            m_materialShader.setUniform("uViewProjectionMatrix", proj * getView(face, probe.getPosition()));
            m_materialShader.setUniform("uCameraPosition", probe.getPosition());

            // TODO: if we do this often, move it into a different function
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
                }
            }

            // Capture distant probe
            // TODO: we're copying code over
            m_ctx.setState(RenderStateBuilder()
                           .setDepthTest(true)
                           .setDepthWrite(false)
                           .setDepthFunc(DepthFuncEnum::LESS_EQUAL)
                           .build());

            m_skyShader.bind();
            m_skyShader.setUniform("uViewProjectionMatrix", proj * getView(face));
            m_skyShader.setUniform("uEnvMap", distantProbe.getEnvironmentMap());

            drawNVertices(14);
        }

        probe.getEnvironmentMap().generateMipmap();

        //-------------------------------
        // diffuse irradiance convolution
        //-------------------------------

        m_ctx.setState(RenderStateBuilder()
                       .setDepthTest(false)
                       .build());

        m_diffuseIrradianceShader.bind();
        m_diffuseIrradianceShader.setUniform("uEnvMap", probe.getEnvironmentMap());

        for (u32 face = 0; face < 6; ++face) {
            // set current face as output color attachment
            m_diffuseIrradianceShader.setUniform("uViewProjectionMatrix", proj * getView(face));
            m_ctx.setRenderTarget(probe.getDiffuseIrradianceCubemap(), (RenderContext::CubemapFaceEnum)face);
            m_ctx.clear(RenderContext::CLEAR_COLOR);

            // draw cube
            drawNVertices(14);
        }

        // update mipmap for diffuse irradiance cubemap
        probe.getDiffuseIrradianceCubemap().generateMipmap();

        //--------------------------
        // prefilter environment map
        //--------------------------

        m_envMapPrefilterShader.bind();
        m_envMapPrefilterShader.setUniform("uEnvMap", probe.getEnvironmentMap());

        for (u32 level = 0; level <= probe.getNumPrefilteredMipmapLevels(); ++level) {
            // set current roughness for prefilter
            f32 roughness = (f32) level / (f32) (probe.getNumPrefilteredMipmapLevels());
            m_envMapPrefilterShader.setUniform("uRoughness", roughness);

            for (u32 face = 0; face < 6; ++face) {
                // set current face as output color attachment
                m_envMapPrefilterShader.setUniform("uViewProjectionMatrix", proj * getView(face));
                m_ctx.setRenderTarget(probe.getPrefilteredCubemap(), (RenderContext::CubemapFaceEnum)face, level);
                m_ctx.clear(RenderContext::CLEAR_COLOR);

                // draw cube
                drawNVertices(14);
            }
        }

        probe.unsetDirty();
    }
}

void Renderer::render(const Scene &scene) {
    m_ctx.setRenderTarget(m_hdrFrameTexture);
    m_ctx.clear(RenderContext::CLEAR_COLOR | RenderContext::CLEAR_DEPTH);

    // draw scene
    {
        m_renderStats = {};

        m_ctx.setState(RenderStateBuilder()
                       .setDepthTest(true)
                       .build());

        m_materialShader.bind();

        // TODO: probe interpolation
        // TODO: actually get closest and not first
        const IblProbe &closestIblProbe = scene.getIblProbes().front();

        m_materialShader.setUniform("uDiffuseIrradianceMap", closestIblProbe.getDiffuseIrradianceCubemap());
        m_materialShader.setUniform("uPrefilteredEnvironmentMap", closestIblProbe.getPrefilteredCubemap());
        m_materialShader.setUniform("uNumPrefilteredEnvMipmapLevels", closestIblProbe.getNumPrefilteredMipmapLevels());

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
                m_materialShader.setUniform("uMaterial.albedo_scale", mesh.getMaterial().albedoScale);
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
        m_skyShader.setUniform("uEnvMap", scene.getIblProbes().front().getEnvironmentMap());

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
