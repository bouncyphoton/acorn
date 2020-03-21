#include "renderer.h"
#include "types.h"
#include "shader.h"
#include <GL/gl3w.h>
#include <cstdio>

static u32 material_shader = 0;

const char *material_vertex_src = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out VertexData {
    vec3 normal;
} o;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

void main() {
    gl_Position = uViewProjectionMatrix * uModelMatrix * vec4(aPosition, 1);
    o.normal = vec3(uNormalMatrix * vec4(aNormal, 1));
})";

const char *material_fragment_src = R"(#version 330 core
layout (location = 0) out vec4 oFragColor;

in VertexData {
    vec3 normal;
} i;

uniform struct {
    vec3 color;
} uMaterial;

void main() {
    vec3 N = normalize(i.normal);
    vec3 L = normalize(vec3(0, 1, 1)); // light direction

    vec3 color = uMaterial.color * max(0, dot(N, L));
    oFragColor = vec4(color, 1);
})";

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    fprintf(severity == GL_DEBUG_SEVERITY_HIGH ? stderr : stdout, "[debug][opengl] %s\n", message);
}

bool renderer_init() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

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

void renderer_draw(renderable *renderables, u32 num_renderables) {
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: optimize by removing redundant binds and uniform setting
    shader_bind(material_shader);

    // camera
    glm::mat4 view_matrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    glm::mat4 projection_matrix = glm::perspective(glm::half_pi<f32>(), 800.0f / 600.0f, 0.001f, 100.0f);
    glm::mat4 view_projection_matrix = projection_matrix * view_matrix;
    shader_set_mat4(material_shader, "uViewProjectionMatrix", view_projection_matrix);

    // render renderables
    for (u32 i = 0; i < num_renderables; ++i) {
        renderable *current = &renderables[i];

        glm::mat4 model_matrix = transform_to_matrix(&current->mesh_transform);
        shader_set_mat4(material_shader, "uModelMatrix", model_matrix);
        shader_set_mat4(material_shader, "uNormalMatrix", glm::transpose(glm::inverse(model_matrix)));
        shader_set_vec3(material_shader, "uMaterial.color", current->mesh_material->color);

        glBindVertexArray(current->mesh_data->vao);
        glDrawArrays(GL_TRIANGLES, 0, current->mesh_data->num_vertices);

    }
}
