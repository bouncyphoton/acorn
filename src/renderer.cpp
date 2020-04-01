#include "renderer.h"
#include "types.h"
#include "framebuffer.h"
#include "shader.h"
#include "utils.h"
#include "texture.h"
#include <stb_image.h>
#include <GL/gl3w.h>
#include <cstdio>

#define RENDERER_DEBUG_CHECKING_ENABLED true
#define MAX_RENDERABLES_PER_FRAME 10

// renderable queuing
static u32 num_renderables_queued = 0;
static Renderable renderables[MAX_RENDERABLES_PER_FRAME] = {};

// textures
static u32 skybox_texture = 0;

// framebuffers
static Framebuffer default_fbo = {};
static Framebuffer diffuse_irradiance_fbo = {};

// shaders
static u32 material_shader = 0;
static u32 sky_shader = 0;

// dummy vao
static u32 dummy_vao = 0;

// render stats
static RenderStats render_stats = {};

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "[error][opengl] %s\n", message);
    } else {
        fprintf(stdout, "[debug][opengl] %s\n", message);
    }
}

bool renderer_init(RenderOptions render_options) {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // TODO: don't hardcode skybox textures into renderer

    // init textures
    s32 w, h;
    void *data[6] = {
            stbi_loadf("../assets/env/px.hdr", &w, &h, nullptr, 3),
            stbi_loadf("../assets/env/nx.hdr", &w, &h, nullptr, 3),
            stbi_loadf("../assets/env/py.hdr", &w, &h, nullptr, 3),
            stbi_loadf("../assets/env/ny.hdr", &w, &h, nullptr, 3),
            stbi_loadf("../assets/env/pz.hdr", &w, &h, nullptr, 3),
            stbi_loadf("../assets/env/nz.hdr", &w, &h, nullptr, 3)
    };
    skybox_texture = texture_cubemap_create(GL_RGB16F, w, h, GL_FLOAT, data, GL_RGB);
    for (int i = 0; i < 6; ++i) {
        free(data[i]);
    }

    // init fbos
    default_fbo = framebuffer_create(
            texture_2d_create(GL_RGB16F, render_options.width, render_options.height, GL_FLOAT, nullptr, GL_RGB), true
    );
    if (default_fbo.id == 0) {
        return false;
    }

    diffuse_irradiance_fbo = framebuffer_create(
            texture_cubemap_create(GL_RGB16F, w, h, GL_FLOAT, nullptr, GL_RGB), false
    );
    if (diffuse_irradiance_fbo.id == 0) {
        return false;
    }

    // init shaders
    char *material_vert = load_file_as_string("../assets/shaders/material.vert");
    char *material_frag = load_file_as_string("../assets/shaders/material.frag");
    material_shader = shader_create(material_vert, material_frag);
    free(material_vert);
    free(material_frag);

    if (material_shader == 0) {
        return false;
    }

    char *sky_vert = load_file_as_string("../assets/shaders/sky.vert");
    char *sky_frag = load_file_as_string("../assets/shaders/sky.frag");
    sky_shader = shader_create(sky_vert, sky_frag);
    free(sky_vert);
    free(sky_frag);

    if (sky_shader == 0) {
        return false;
    }

    glGenVertexArrays(1, &dummy_vao);
    if (dummy_vao == 0) {
        fprintf(stderr, "[error] failed to generate dummy vao\n");
        return false;
    }

    return true;
}

void renderer_shutdown() {
    shader_destroy(material_shader);
    framebuffer_destroy(&default_fbo);
}

void renderer_queue_renderable(Renderable renderable) {
    if (num_renderables_queued >= MAX_RENDERABLES_PER_FRAME) {
        fprintf(stderr, "[warn] tried to queue more renderables than MAX_RENDERABLES (%d)\n",
                MAX_RENDERABLES_PER_FRAME);
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

static void renderer_draw_scene(GameState *game_state) {
    render_stats = {};

    // TODO: optimize by removing redundant binds and uniform setting
    shader_bind(material_shader);

    u32 texture_idx = 0;

    // TODO: diffuse irradiance map generation
    // TODO: prefiltered environment map generation
    // TODO: prefilter mipmap resolution in fragment shader
    // TODO: brdf lut generation

    glActiveTexture(GL_TEXTURE0 + texture_idx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_irradiance_fbo.texture);
    shader_set_int(material_shader, "uDiffuseIrradianceMap", texture_idx);
    ++texture_idx;

    // sun
    shader_set_vec3(material_shader, "uSunDirection", game_state->sun_direction);

    // camera
    f32 aspect_ratio = (f32) game_state->render_options.width / game_state->render_options.height;
    glm::mat4 view_matrix = glm::lookAt(game_state->camera.position, game_state->camera.look_at, glm::vec3(0, 1, 0));
    glm::mat4 projection_matrix = glm::perspective(game_state->camera.fov_radians, aspect_ratio, 0.001f, 1000.0f);
    glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
    shader_set_mat4(material_shader, "uViewProjectionMatrix", view_projection_matrix);
    shader_set_vec3(material_shader, "uCameraPosition", game_state->camera.position);

    // render renderables
    for (u32 i = 0; i < num_renderables_queued; ++i) {
        Renderable *current = &renderables[i];

        glm::mat4 model_matrix = transform_to_matrix(&current->transform);
        shader_set_mat4(material_shader, "uModelMatrix", model_matrix);
        shader_set_mat4(material_shader, "uNormalMatrix", glm::transpose(glm::inverse(model_matrix)));

        // render each mesh in model
        for (u32 m = 0; m < current->model->num_meshes; ++m) {
            Material *material = &current->model->materials[m];
            Mesh *mesh = &current->model->meshes[m];

            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glBindTexture(GL_TEXTURE_2D, material->albedo_texture);
            shader_set_int(material_shader, "uMaterial.albedo", texture_idx);
            ++texture_idx;

            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glBindTexture(GL_TEXTURE_2D, material->normal_texture);
            shader_set_int(material_shader, "uMaterial.normal", texture_idx);
            ++texture_idx;

            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glBindTexture(GL_TEXTURE_2D, material->metallic_texture);
            shader_set_int(material_shader, "uMaterial.metallic", texture_idx);
            ++texture_idx;

            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glBindTexture(GL_TEXTURE_2D, material->roughness_texture);
            shader_set_int(material_shader, "uMaterial.roughness", texture_idx);
            ++texture_idx;

            glBindVertexArray(mesh->vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh->num_vertices);

            ++render_stats.draw_calls;
            render_stats.vertices_rendered += mesh->num_vertices;
        }
    }

    // reset queued renderables
    num_renderables_queued = 0;
}

static void renderer_draw_sky(GameState *game_state) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    shader_bind(sky_shader);

    f32 aspect_ratio = game_state->render_options.width / (f32)game_state->render_options.height;

    shader_set_mat4(sky_shader, "uViewProjectionMatrix",
                    glm::perspective(game_state->camera.fov_radians, aspect_ratio, 0.01f, 10.0f) *
                    glm::lookAt(glm::vec3(0), game_state->camera.look_at - game_state->camera.position,
                                glm::vec3(0, 1, 0)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
    shader_set_int(sky_shader, "uSkybox", 0);

    glBindVertexArray(dummy_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void renderer_draw(GameState *game_state) {
    // bind default fbo
    framebuffer_bind(&default_fbo);
    s32 width, height;
    texture_2d_get_dimensions(default_fbo.texture, 0, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw scene
    renderer_draw_scene(game_state);

    // draw sky
    renderer_draw_sky(game_state);

    // blit to default framebuffer
    framebuffer_blit_to_default_framebuffer(&default_fbo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

RenderStats renderer_get_stats() {
    return render_stats;
}
