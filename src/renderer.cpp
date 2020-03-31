#include "renderer.h"
#include "types.h"
#include "framebuffer.h"
#include "shader.h"
#include "utils.h"
#include <GL/gl3w.h>
#include <cstdio>

#define RENDERER_DEBUG_CHECKING_ENABLED true
#define MAX_RENDERABLES_PER_FRAME 10

// renderable queuing
static u32 num_renderables_queued = 0;
static Renderable renderables[MAX_RENDERABLES_PER_FRAME] = {};

// framebuffers
static Framebuffer default_fbo = {};

// shaders
static u32 material_shader = 0;

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

    // init fbos
    default_fbo = framebuffer_create(GL_RGBA, render_options.width, render_options.height);
    if (default_fbo.id == 0) {
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

    return true;
}

void renderer_shutdown() {
    shader_destroy(material_shader);
    framebuffer_destroy(&default_fbo);
}

void renderer_queue_renderable(Renderable renderable) {
    if (num_renderables_queued >= MAX_RENDERABLES_PER_FRAME) {
        fprintf(stderr, "[warn] tried to queue more renderables than MAX_RENDERABLES (%d)\n", MAX_RENDERABLES_PER_FRAME);
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

void renderer_draw(GameState *game_state) {
    render_stats = {};

    // bind default fbo
    framebuffer_bind(&default_fbo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: optimize by removing redundant binds and uniform setting
    shader_bind(material_shader);

    // sun
    shader_set_vec3(material_shader, "uSunDirection", game_state->sun_direction);

    // camera
    f32 aspect_ratio = (f32)game_state->render_options.width / game_state->render_options.height;
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

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->albedo_texture);
            shader_set_int(material_shader, "uMaterial.albedo", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material->normal_texture);
            shader_set_int(material_shader, "uMaterial.normal", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material->metallic_texture);
            shader_set_int(material_shader, "uMaterial.metallic", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, material->roughness_texture);
            shader_set_int(material_shader, "uMaterial.roughness", 3);

            glBindVertexArray(mesh->vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh->num_vertices);

            ++render_stats.draw_calls;
            render_stats.vertices_rendered += mesh->num_vertices;
        }
    }

    // reset queued renderables
    num_renderables_queued = 0;

    // blit to default framebuffer
    framebuffer_blit_to_default_framebuffer(&default_fbo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

RenderStats renderer_get_stats() {
    return render_stats;
}
