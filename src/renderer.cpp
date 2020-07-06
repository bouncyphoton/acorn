#include "renderer.h"
#include "types.h"
#include "shader.h"
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

void Renderer::init() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

    // set opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // TODO: figure out why enabling GL_BLEND breaks rendering to cubemap framebuffer for diffuse convolve
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    m_environmentMap.initCubemap(GL_RGB16F, w, h, GL_FLOAT, data, GL_RGB);
    for (int i = 0; i < 6; ++i) {
        stbi_image_free(data[i]);
    }
    stbi_set_flip_vertically_on_load(1);

    m_diffuseIrradianceCubemap.initCubemap(GL_RGB16F,
                                           consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE,
                                           consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE,
                                           GL_FLOAT, nullptr, GL_RGB);
    m_prefilteredEnvCubemap.initCubemap(GL_RGB16F,
                                        consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE,
                                        consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE,
                                        GL_FLOAT, nullptr, GL_RGB);
    m_brdfLut.init2D(GL_RG16F,
                     consts::BRDF_LUT_TEXTURE_SIZE,
                     consts::BRDF_LUT_TEXTURE_SIZE,
                     GL_FLOAT, nullptr, GL_RG);
    m_workingTexture.init2D(GL_RGB16F, core->gameState.renderOptions.width,
                            core->gameState.renderOptions.height, GL_FLOAT, nullptr, GL_RGB);
    m_defaultFboTexture.init2D(GL_RGB16F, core->gameState.renderOptions.width,
                               core->gameState.renderOptions.height, GL_FLOAT, nullptr, GL_RGB);

    //----------
    // init fbos
    //----------

    m_workingFbo.init(w, h);
    m_defaultFbo.init(core->gameState.renderOptions.width, core->gameState.renderOptions.height);

    if (!m_defaultFbo.id || !m_workingFbo.id) {
        core->fatal("Failed to create FBOs");
    }

    m_defaultFbo.bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_defaultFboTexture.id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-------------
    // init shaders
    //-------------

    std::string dir = "../assets/shaders/";
    m_materialShader.init(dir + "material.vert", dir + "material.frag");
    m_skyShader.init(dir + "cube.vert", dir + "sky.frag");
    m_diffuseIrradianceShader.init(dir + "cube.vert", dir + "diffuse_irradiance_convolution.frag");
    m_envMapPrefilterShader.init(dir + "cube.vert", dir + "env_map_prefilter.frag");
    m_brdfLutShader.init(dir + "fullscreen.vert", dir + "brdf_lut.frag");
    m_tonemapShader.init(dir + "fullscreen.vert", dir + "tonemap.frag");

    //----------
    // dummy vao
    //----------

    glGenVertexArrays(1, &m_dummyVao);
    if (m_dummyVao == 0) {
        core->fatal("Failed to generate dummy VAO");
    }

    //-----------
    // precompute
    //-----------

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap.id);
    m_diffuseIrradianceShader.setInt("uEnvMap", 0);

    for (u32 face = 0; face < 6; ++face) {
        // set current face as output color attachment
        m_diffuseIrradianceShader.setMat4("uViewProjectionMatrix", proj * views[face]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                               m_diffuseIrradianceCubemap.id, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw cube
        glBindVertexArray(m_dummyVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
        glBindVertexArray(0);
    }

    // update mipmaps for diffuse irradiance cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_diffuseIrradianceCubemap.id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    //--------------------------
    // prefilter environment map
    //--------------------------

    m_envMapPrefilterShader.bind();

    // set environment map uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap.id);
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
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                   m_prefilteredEnvCubemap.id, level);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw cube
            glBindVertexArray(m_dummyVao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
            glBindVertexArray(0);
        }
    }

    //---------
    // brdf lut
    //---------

    m_brdfLutShader.bind();
    glViewport(0, 0, consts::BRDF_LUT_TEXTURE_SIZE, consts::BRDF_LUT_TEXTURE_SIZE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLut.id, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quad
    glBindVertexArray(m_dummyVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // update mipmaps for brdf lut
    glBindTexture(GL_TEXTURE_2D, m_brdfLut.id);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set state for normal rendering
    m_workingFbo.destroy();
    m_workingFbo.init(core->gameState.renderOptions.width, core->gameState.renderOptions.height);
    m_workingFbo.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_workingTexture.id, 0);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::destroy() {
    m_brdfLutShader.destroy();
    m_envMapPrefilterShader.destroy();
    m_diffuseIrradianceShader.destroy();
    m_skyShader.destroy();
    m_materialShader.destroy();

    m_brdfLut.destroy();
    m_prefilteredEnvCubemap.destroy();
    m_diffuseIrradianceCubemap.destroy();
    m_environmentMap.destroy();
    m_workingTexture.destroy();
    m_defaultFboTexture.destroy();

    m_workingFbo.destroy();
    m_defaultFbo.destroy();
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

        // TODO: take tonemapping out of sky.frag and material.frag to separate pass to keep everything linear

        glActiveTexture(GL_TEXTURE0 + textureIdx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_diffuseIrradianceCubemap.id);
        m_materialShader.setInt("uDiffuseIrradianceMap", textureIdx);
        ++textureIdx;

        glActiveTexture(GL_TEXTURE0 + textureIdx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilteredEnvCubemap.id);
        m_materialShader.setInt("uPrefilteredEnvironmentMap", textureIdx);
        ++textureIdx;

        m_materialShader.setInt("uNumPrefilteredEnvMipmapLevels", m_numPrefilteredEnvMipmapLevels);

        glActiveTexture(GL_TEXTURE0 + textureIdx);
        glBindTexture(GL_TEXTURE_2D, m_brdfLut.id);
        m_materialShader.setInt("uBrdfLut", textureIdx);
        ++textureIdx;

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
            for (auto &mesh : entity.model->meshes) {
                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.albedoTexture);
                m_materialShader.setInt("uMaterial.albedo", textureIdx);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.normalTexture);
                m_materialShader.setInt("uMaterial.normal", textureIdx);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.metallicTexture);
                m_materialShader.setInt("uMaterial.metallic", textureIdx);
                m_materialShader.setFloat("uMaterial.metallic_scale", mesh.material.metallicScale);
                ++textureIdx;

                glActiveTexture(GL_TEXTURE0 + textureIdx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.roughnessTexture);
                m_materialShader.setInt("uMaterial.roughness", textureIdx);
                m_materialShader.setFloat("uMaterial.roughness_scale", mesh.material.roughnessScale);
                ++textureIdx;

                glBindVertexArray(mesh.vao);
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());

                ++m_renderStats.drawCalls;
                m_renderStats.verticesRendered += mesh.vertices.size();
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap.id);
        m_skyShader.setInt("uEnvMap", 0);

        glBindVertexArray(m_dummyVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    // tonemap
    {
        // workingFbo.data = defaultFbo.data
        m_defaultFbo.blit(m_workingFbo, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        m_tonemapShader.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_workingTexture.id);
        m_tonemapShader.setInt("uImage", 0);
        m_tonemapShader.setFloat("uExposure", core->gameState.camera.exposure);

        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(m_dummyVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    // blit to default framebuffer
    m_defaultFbo.blitToDefaultFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderStats Renderer::getStats() {
    return m_renderStats;
}
