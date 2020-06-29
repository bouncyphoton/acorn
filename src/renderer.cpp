#include "renderer.h"
#include "types.h"
#include "framebuffer.h"
#include "shader.h"
#include "utils.h"
#include "texture.h"
#include "core.h"
#include "constants.h"
#include <stb_image.h>
#include <GL/gl3w.h>
#include <cstdio>

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "[error][opengl] %s\n", message);
    } else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stdout, "[debug][opengl] %s\n", message);
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
    // NOTE: this code runs before textures_init, so image isn't flipped (which in this case, is good)
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
        free(data[i]);
    }

    m_diffuseIrradianceCubemap.initCubemap(GL_RGB16F,
                                    consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE,
                                    consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE,
                                    GL_FLOAT, nullptr, GL_RGB);
    m_prefilteredEnvCubemap.initCubemap(GL_RGB16F,
                                 consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE,
                                 consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE,
                                 GL_FLOAT, nullptr, GL_RGB);
    m_brdfLut.init2d(GL_RG16F,
                   consts::BRDF_LUT_TEXTURE_SIZE,
                   consts::BRDF_LUT_TEXTURE_SIZE,
                   GL_FLOAT, nullptr, GL_RG);
    m_defaultFboTexture.init2d(GL_RGB16F, core->game_state.render_options.width,
                                            core->game_state.render_options.height, GL_FLOAT, nullptr, GL_RGB);

    //----------
    // init fbos
    //----------

    m_workingFbo.init(w, h);
    m_defaultFbo.init(core->game_state.render_options.width, core->game_state.render_options.height);

    if (!m_defaultFbo.id || !m_workingFbo.id) {
        core->fatal("Failed to create FBOs");
    }

    m_defaultFbo.bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_defaultFboTexture.id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-------------
    // init shaders
    //-------------

    // TODO: decide if this is ugly or not
    // the verdict is in: yeah

    std::string dir = "../assets/shaders/";
    m_materialShader.init(dir + "material.vert", dir + "material.frag");
    m_skyShader.init(dir + "cube.vert", dir + "sky.frag");
    m_diffuseIrradianceShader.init(dir + "cube.vert", dir + "diffuse_irradiance_convolution.frag");
    m_envMapPrefilterShader.init(dir + "cube.vert", dir + "env_map_prefilter.frag");
    m_brdfLutShader.init(dir + "fullscreen.vert", dir + "brdf_lut.frag");

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
    m_defaultFboTexture.destroy();

    m_workingFbo.destroy();
    m_defaultFbo.destroy();
}

void Renderer::queueRenderable(Renderable renderable) {
    if (num_renderables_queued >= consts::MAX_RENDERABLES) {
        fprintf(stderr, "[warn] tried to queue more renderables than MAX_RENDERABLES (%d)\n",
                consts::MAX_RENDERABLES);
        return;
    }

#if RENDERER_DEBUG_CHECKING_ENABLED
    if (renderable.model == nullptr) {
        fprintf(stderr, "[error] tried to queue renderable with null model\n");
        return;
    }

    if (renderable.model->meshes == nullptr || renderable.model->materials == 0) {
        fprintf(stderr, "[error] tried to queue renderable with invalid model:\n"
                        "Model {\n"
                        "  .meshes     = %p\n"
                        "  .materials  = %p\n"
                        "}\n",
                renderable.model->meshes, renderable.model->materials);
        return;
    }
#endif

    // queue renderable
    renderables[num_renderables_queued] = renderable;
    ++num_renderables_queued;
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

        u32 texture_idx = 0;

        // TODO: take tonemapping out of sky.frag and material.frag to separate pass to keep everything linear

        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_diffuseIrradianceCubemap.id);
        m_materialShader.setInt("uDiffuseIrradianceMap", texture_idx);
        ++texture_idx;

        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilteredEnvCubemap.id);
        m_materialShader.setInt("uPrefilteredEnvironmentMap", texture_idx);
        ++texture_idx;

        m_materialShader.setInt("uNumPrefilteredEnvMipmapLevels", m_numPrefilteredEnvMipmapLevels);

        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glBindTexture(GL_TEXTURE_2D, m_brdfLut.id);
        m_materialShader.setInt("uBrdfLut", texture_idx);
        ++texture_idx;

        // sun
        m_materialShader.setVec3("uSunDirection", core->game_state.sun_direction);

        // camera
        f32 aspect_ratio = (f32) core->game_state.render_options.width / core->game_state.render_options.height;
        glm::mat4 view_matrix = glm::lookAt(core->game_state.camera.position, core->game_state.camera.look_at,
                                            glm::vec3(0, 1, 0));
        glm::mat4 projection_matrix = glm::perspective(core->game_state.camera.fov_radians, aspect_ratio, 0.001f,
                                                       1000.0f);
        glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
        m_materialShader.setMat4("uViewProjectionMatrix", view_projection_matrix);
        m_materialShader.setVec3("uCameraPosition", core->game_state.camera.position);

        // render renderables
        for (u32 i = 0; i < num_renderables_queued; ++i) {
            Renderable *current = &renderables[i];

            glm::mat4 model_matrix = transform_to_matrix(&current->transform);
            m_materialShader.setMat4("uModelMatrix", model_matrix);
            m_materialShader.setMat4("uNormalMatrix", glm::transpose(glm::inverse(model_matrix)));

            // render each mesh in model
            for (auto &mesh : current->model->meshes) {
                glActiveTexture(GL_TEXTURE0 + texture_idx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.albedo_texture);
                m_materialShader.setInt("uMaterial.albedo", texture_idx);
                ++texture_idx;

                glActiveTexture(GL_TEXTURE0 + texture_idx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.normal_texture);
                m_materialShader.setInt("uMaterial.normal", texture_idx);
                ++texture_idx;

                glActiveTexture(GL_TEXTURE0 + texture_idx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.metallic_texture);
                m_materialShader.setInt("uMaterial.metallic", texture_idx);
                m_materialShader.setFloat("uMaterial.metallic_scale", mesh.material.metallic_scale);
                ++texture_idx;

                glActiveTexture(GL_TEXTURE0 + texture_idx);
                glBindTexture(GL_TEXTURE_2D, mesh.material.roughness_texture);
                m_materialShader.setInt("uMaterial.roughness", texture_idx);
                m_materialShader.setFloat("uMaterial.roughness_scale", mesh.material.roughness_scale);
                ++texture_idx;

                glBindVertexArray(mesh.vao);
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());

                ++m_renderStats.draw_calls;
                m_renderStats.vertices_rendered += mesh.vertices.size();
            }
        }

        // reset queued renderables
        num_renderables_queued = 0;
    }

    // draw sky
    {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        m_skyShader.bind();

        f32 aspect_ratio = core->game_state.render_options.width / (f32) core->game_state.render_options.height;

        m_skyShader.setMat4("uViewProjectionMatrix",
                        glm::perspective(core->game_state.camera.fov_radians, aspect_ratio, 0.01f, 10.0f) *
                        glm::lookAt(glm::vec3(0), core->game_state.camera.look_at - core->game_state.camera.position,
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

    // blit to default framebuffer
    m_defaultFbo.blitToDefaultFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

RenderStats Renderer::getStats() {
    return m_renderStats;
}
