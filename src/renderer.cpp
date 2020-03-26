#include "renderer.h"
#include "types.h"
#include "shader.h"
#include <GL/gl3w.h>
#include <cstdio>

#define RENDERER_DEBUG_CHECKING_ENABLED true
#define MAX_RENDERABLES_PER_FRAME 1

// renderable queuing
static u32 num_renderables_queued = 0;
static Renderable renderables[MAX_RENDERABLES_PER_FRAME] = {};

// shaders
static u32 material_shader = 0;

// TODO: don't hardcode shader sources
const char *material_vertex_src = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out VertexData {
    vec3 normal;
    vec3 color;
} o;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

void main() {
    gl_Position = uViewProjectionMatrix * uModelMatrix * vec4(aPosition, 1);
    o.normal = vec3(uNormalMatrix * vec4(aNormal, 1));
    o.color = aColor;
})";
const char *material_fragment_src = R"(#version 330 core
layout (location = 0) out vec4 oFragColor;

in VertexData {
    vec3 normal;
    vec3 color;
} i;

uniform struct {
    vec3 color;
} uMaterial;

void main() {
    vec3 N = normalize(i.normal);
    vec3 L = normalize(vec3(0, 1, 1)); // light direction

    vec3 color = i.color * uMaterial.color * max(0.1, dot(N, L));
    oFragColor = vec4(color, 1);
})";

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "[error][opengl] %s\n", message);
    } else {
        fprintf(stdout, "[debug][opengl] %s\n", message);
    }
}

bool renderer_init() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

    glEnable(GL_DEPTH_TEST);

    // init shaders
    material_shader = shader_create(material_vertex_src, material_fragment_src);
    if (material_shader == 0) {
        return false;
    }

    return true;
}

void renderer_shutdown() {
    shader_destroy(material_shader);
}

void renderer_queue_renderable(Renderable renderable) {
    if (num_renderables_queued >= MAX_RENDERABLES_PER_FRAME) {
        fprintf(stderr, "[warn] tried to queue more renderables than MAX_RENDERABLES (%d)\n", MAX_RENDERABLES_PER_FRAME);
        return;
    }

#if RENDERER_DEBUG_CHECKING_ENABLED
    if (renderable.material == nullptr || renderable.mesh == nullptr) {
        fprintf(stderr, "[error] tried to queue invalid renderable:\n"
                        "Renderable {\n"
                        "  .mesh     = %p\n"
                        "  .material = %p\n"
                        "}\n",
                renderable.mesh, renderable.material);
        return;
    }

    if (renderable.mesh->vertices == nullptr || renderable.mesh->num_vertices == 0
        || renderable.mesh->vao == 0 || renderable.mesh->vbo == 0) {
        fprintf(stderr, "[error] tried to queue renderable with invalid mesh:\n"
                        "Mesh {\n"
                        "  .vertices     = %p\n"
                        "  .num_vertices = %u\n"
                        "  .vao          = %u\n"
                        "  .vbo          = %u\n"
                        "}\n",
                renderable.mesh->vertices, renderable.mesh->num_vertices, renderable.mesh->vao, renderable.mesh->vbo);
        return;
    }
#endif

    // queue renderable
    renderables[num_renderables_queued] = renderable;
    ++num_renderables_queued;
}

void renderer_draw(GameState *game_state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: optimize by removing redundant binds and uniform setting
    shader_bind(material_shader);

    // camera
    f32 aspect_ratio = (f32)game_state->render_options.width / game_state->render_options.height;
    glm::mat4 view_matrix = glm::lookAt(game_state->camera.position, game_state->camera.look_at, glm::vec3(0, 1, 0));
    glm::mat4 projection_matrix = glm::perspective(glm::half_pi<f32>(), aspect_ratio, 0.001f, 100.0f);
    glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
    shader_set_mat4(material_shader, "uViewProjectionMatrix", view_projection_matrix);

    // render renderables
    for (u32 i = 0; i < num_renderables_queued; ++i) {
        Renderable *current = &renderables[i];

        glm::mat4 model_matrix = transform_to_matrix(&current->transform);
        shader_set_mat4(material_shader, "uModelMatrix", model_matrix);
        shader_set_mat4(material_shader, "uNormalMatrix", glm::transpose(glm::inverse(model_matrix)));
        shader_set_vec3(material_shader, "uMaterial.color", current->material->color);

        glBindVertexArray(current->mesh->vao);
        glDrawArrays(GL_TRIANGLES, 0, current->mesh->num_vertices);
    }

    // reset queued renderables
    num_renderables_queued = 0;
}
