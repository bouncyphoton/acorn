#include "renderer.h"
#include "types.h"
#include "utils.h"
#include "core.h"
#include "constants.h"
#include <stb_image.h>
#include <GL/gl3w.h>

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        core->warn("[error][opengl] " + std::string(message));
    } else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        core->info("[opengl] " + std::string(message));
    }
}

// TODO: load shaders from resource manager instead
#define SHADER_DIR "../assets/shaders/"

Renderer::Renderer()
        : m_materialShader(SHADER_DIR "material.vert", SHADER_DIR "material.frag"),
          m_skyShader(SHADER_DIR "cube.vert", SHADER_DIR "sky.frag"),
          m_diffuseIrradianceShader(SHADER_DIR "cube.vert", SHADER_DIR "diffuse_irradiance_convolution.frag"),
          m_envMapPrefilterShader(SHADER_DIR "cube.vert", SHADER_DIR "env_map_prefilter.frag"),
          m_brdfLutShader(SHADER_DIR "fullscreen.vert", SHADER_DIR "brdf_lut.frag"),
          m_tonemapShader(SHADER_DIR "fullscreen.vert", SHADER_DIR "tonemap.frag") {
    core->debug("Renderer::Renderer()");
    init();
    precompute();
}

Renderer::~Renderer() {
    core->debug("Renderer::~Renderer()");
}

void Renderer::init() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

    // set opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
        core->fatal("skybox side textures are not square");
    }
    m_environmentMap.setImage(w, TextureFormatEnum::RGB16F, data);
    for (int i = 0; i < 6; ++i) {
        stbi_image_free(data[i]);
    }
    stbi_set_flip_vertically_on_load(1);

    m_diffuseIrradianceCubemap.setImage(consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_prefilteredEnvCubemap.setImage(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_brdfLut.setImage(consts::BRDF_LUT_TEXTURE_SIZE, consts::BRDF_LUT_TEXTURE_SIZE, TextureFormatEnum::RG16F);
    m_workingTexture.setImage(core->gameState.renderOptions.width, core->gameState.renderOptions.height,
                              TextureFormatEnum::RGB16F);
    m_defaultFboTexture.setImage(core->gameState.renderOptions.width, core->gameState.renderOptions.height,
                                 TextureFormatEnum::RGB16F);

    //----------
    // init fbos
    //----------

    m_workingFbo.attachTexture(m_workingTexture);
    m_defaultFbo.attachTexture(m_defaultFboTexture);

    //----------
    // dummy vao
    //----------

    glGenVertexArrays(1, &m_dummyVao);
    if (m_dummyVao == 0) {
        core->fatal("Failed to generate dummy VAO");
    }
}

void Renderer::precompute() {
    // set state for precomputations
    m_workingFbo.bind();
    glDisable(GL_DEPTH_TEST);

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

    //-------------------------------
    // diffuse irradiance convolution
    //-------------------------------

    m_diffuseIrradianceShader.bind();
    glViewport(0, 0, consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE);

    // set environment map uniform
    m_environmentMap.bind(0);
    m_diffuseIrradianceShader.setInt("uEnvMap", 0);

    for (u32 face = 0; face < 6; ++face) {
        // set current face as output color attachment
        m_diffuseIrradianceShader.setMat4("uViewProjectionMatrix", proj * views[face]);
        m_workingFbo.attachTexture(m_diffuseIrradianceCubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw cube
        drawNVertices(14);
    }

    // update mipmaps for diffuse irradiance cubemap
    m_diffuseIrradianceCubemap.bind(0);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    //--------------------------
    // prefilter environment map
    //--------------------------

    m_envMapPrefilterShader.bind();

    // set environment map uniform
    m_environmentMap.bind(0);
    m_envMapPrefilterShader.setInt("uEnvMap", 0);

    // calculate mipmap levels
    m_numPrefilteredEnvMipmapLevels = floor(log2(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE));

    for (u32 level = 0; level <= m_numPrefilteredEnvMipmapLevels; ++level) {
        // set current roughness for prefilter
        f32 roughness = (f32) level / (f32) (m_numPrefilteredEnvMipmapLevels);
        m_envMapPrefilterShader.setFloat("uRoughness", roughness);

        u32 size = consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE * pow(0.5f, level);
        glViewport(0, 0, size, size);

        for (u32 face = 0; face < 6; ++face) {
            // set current face as output color attachment
            m_envMapPrefilterShader.setMat4("uViewProjectionMatrix", proj * views[face]);
            m_workingFbo.attachTexture(m_prefilteredEnvCubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw cube
            drawNVertices(14);
        }
    }

    //---------
    // brdf lut
    //---------

    m_brdfLutShader.bind();
    m_workingFbo.attachTexture(m_brdfLut);
    glViewport(0, 0, consts::BRDF_LUT_TEXTURE_SIZE, consts::BRDF_LUT_TEXTURE_SIZE);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quad
    drawNVertices(4);

    // update mipmaps for brdf lut
    m_brdfLut.bind(0);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set state for normal rendering
    m_workingFbo.attachTexture(m_workingTexture);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render() {
    // bind default fbo
    m_defaultFbo.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw scene
    {
        m_renderStats = {};

        // TODO: optimize by removing redundant binds and uniform setting
        m_materialShader.bind();

        u32 textureIdx = 0;

        m_diffuseIrradianceCubemap.bind(textureIdx);
        m_materialShader.setInt("uDiffuseIrradianceMap", textureIdx);
        ++textureIdx;

        m_prefilteredEnvCubemap.bind(textureIdx);
        m_materialShader.setInt("uPrefilteredEnvironmentMap", textureIdx);
        ++textureIdx;

        m_materialShader.setInt("uNumPrefilteredEnvMipmapLevels", m_numPrefilteredEnvMipmapLevels);

        m_brdfLut.bind(textureIdx);
        m_materialShader.setInt("uBrdfLut", textureIdx);
        ++textureIdx;

        const u32 startTextureIdx = textureIdx;

        // sun
        m_materialShader.setVec3("uSunDirection", core->gameState.scene.sunDirection);

        // camera
        f32 aspectRatio = (f32) core->gameState.renderOptions.width / core->gameState.renderOptions.height;
        glm::mat4 viewMatrix = glm::lookAt(core->gameState.camera.position, core->gameState.camera.lookAt,
                                           glm::vec3(0, 1, 0));
        glm::mat4 projectionMatrix = glm::perspective(core->gameState.camera.fovRadians, aspectRatio, 0.001f,
                                                      1000.0f);
        glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
        m_materialShader.setMat4("uViewProjectionMatrix", viewProjectionMatrix);
        m_materialShader.setVec3("uCameraPosition", core->gameState.camera.position);

        // render entities
        for (const Entity &entity : core->gameState.scene.getEntities()) {
            if (!entity.active) continue;

            glm::mat4 modelMatrix = transform_to_matrix(entity.transform);
            m_materialShader.setMat4("uModelMatrix", modelMatrix);
            m_materialShader.setMat4("uNormalMatrix", glm::transpose(glm::inverse(modelMatrix)));

            // render each mesh in model
            for (auto &mesh : entity.model->getMeshes()) {
                textureIdx = startTextureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.getMaterial().albedoTexture);
                m_materialShader.setInt("uMaterial.albedo", textureIdx);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.getMaterial().normalTexture);
                m_materialShader.setInt("uMaterial.normal", textureIdx);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.getMaterial().metallicTexture);
                m_materialShader.setInt("uMaterial.metallic", textureIdx);
                m_materialShader.setFloat("uMaterial.metallic_scale", mesh.getMaterial().metallicScale);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.getMaterial().roughnessTexture);
                m_materialShader.setInt("uMaterial.roughness", textureIdx);
                m_materialShader.setFloat("uMaterial.roughness_scale", mesh.getMaterial().roughnessScale);
                ++textureIdx;

                mesh.draw();

                ++m_renderStats.drawCalls;
                m_renderStats.verticesRendered += mesh.getNumVertices();
            }
        }
    }

    // draw sky
    {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        m_skyShader.bind();

        f32 aspectRatio = core->gameState.renderOptions.width / (f32) core->gameState.renderOptions.height;

        m_skyShader.setMat4("uViewProjectionMatrix",
                            glm::perspective(core->gameState.camera.fovRadians, aspectRatio, 0.01f, 10.0f) *
                            glm::lookAt(glm::vec3(0),
                                        core->gameState.camera.lookAt - core->gameState.camera.position,
                                        glm::vec3(0, 1, 0)));

        m_environmentMap.bind(0);
        m_skyShader.setInt("uEnvMap", 0);

        drawNVertices(14);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    // tonemap
    {
        // workingFbo.data = defaultFbo.data
        m_defaultFbo.blit(m_workingFbo, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        m_tonemapShader.bind();

        m_workingTexture.bind(0);
        m_tonemapShader.setInt("uImage", 0);
        m_tonemapShader.setFloat("uExposure", core->gameState.camera.exposure);

        glDisable(GL_DEPTH_TEST);

        drawNVertices(4);

        glEnable(GL_DEPTH_TEST);
    }

    // blit to default framebuffer
    m_defaultFbo.blitToDefaultFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

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
