#include "mesh.h"
#include <GL/gl3w.h>

// TODO: move away from OBJs at some point in favor of a more elegant and efficient solution
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#undef min
#undef max

Mesh mesh_load(const char *obj_path, const char *mtl_dir) {
    Mesh mesh = {};

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path, mtl_dir, true);
    if (!warn.empty()) {
        printf("[warn] %s\n", warn.c_str());
    }
    if (!err.empty()) {
        fprintf(stderr, "[error] %s\n", err.c_str());
    }
    if (!result) {
        return mesh;
    }

    mesh.num_vertices = shapes[0].mesh.num_face_vertices.size() * 3;
    mesh.vertices = (Vertex *) malloc(sizeof(Vertex) * mesh.num_vertices);
    if (!mesh.vertices) {
        fprintf(stderr, "[error] failed to allocate memory for mesh vertices\n");
        return mesh;
    }

    Vertex *current = mesh.vertices;

    // each shape, but we'll do only one
    for (u32 s = 0; s < (shapes.empty() ? 0 : 1); ++s) {
        u32 index_offset = 0;
        // each face
        for (u32 f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
            int num_verts_per_face = shapes[s].mesh.num_face_vertices[f];

            // each vertex
            for (u32 v = 0; v < num_verts_per_face; ++v) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                current->position = glm::vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                );
                current->normal = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                );
                current->color = glm::vec3(
                        materials[shapes[s].mesh.material_ids[f]].diffuse[0],
                        materials[shapes[s].mesh.material_ids[f]].diffuse[1],
                        materials[shapes[s].mesh.material_ids[f]].diffuse[2]
                );

                mesh.min = glm::min(mesh.min, current->position);
                mesh.max = glm::max(mesh.max, current->position);

                ++current;
            }
            index_offset += num_verts_per_face;
        }
    }

    // create vao and vbo for rendering
    glGenVertexArrays(1, &mesh.vao);
    if (mesh.vao == 0) {
        fprintf(stderr, "[error] failed to generate vao for mesh\n");
        return mesh;
    }
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    if (mesh.vbo == 0) {
        fprintf(stderr, "[error] failed to generate vbo for mesh\n");
        return mesh;
    }
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * sizeof(Vertex), mesh.vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, color));

    glBindVertexArray(0);

    return mesh;
}

void mesh_free(Mesh *mesh) {
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteVertexArrays(1, &mesh->vao);
    free(mesh->vertices);
}
