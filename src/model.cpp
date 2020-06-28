#include "model.h"
#include "texture.h"
#include "core.h"
#include <GL/gl3w.h>

// TODO: move away from OBJs at some point in favor of a more elegant and efficient way of store model data
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#undef min
#undef max
//
//void Model::loadFromObj(const char *obj_path, const char *mtl_dir) {
//    printf("[info] loading model \"%s\"\n", obj_path);
//    tinyobj::attrib_t attrib;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string warn, err;
//
//    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path, mtl_dir, true);
//    if (!warn.empty()) {
//        printf("[warn] %s\n", warn.c_str());
//    }
//    if (!err.empty()) {
//        fprintf(stderr, "[error] %s\n", err.c_str());
//    }
//    if (!result) {
//        core->fatal("Failed to load OBJ '" + std::string(obj_path) + "'");
//    }
//
//    m_numMeshes = materials.size();
//    // we require at least one materials
//    if (m_numMeshes == 0) {
//        m_numMeshes = 1;
//    }
//
//    m_meshes = (Mesh *) malloc(sizeof(Mesh) * m_numMeshes);
//    m_materials = (Material *) malloc(sizeof(Material) * m_numMeshes);
//    if (m_meshes == nullptr || m_materials == nullptr) {
//        core->fatal("Failed to allocate memory for model meshes or materials");
//    }
//
//    // default init
//    for (u32 i = 0; i < m_numMeshes; ++i) {
//        m_meshes[i] = {};
//        m_materials[i] = {};
//
//        if (!materials.empty()) {
//            std::string dir = std::string(mtl_dir) + "/";
//
//            // albedo
//            std::string albedo_path = dir + materials[i].diffuse_texname;
//            m_materials[i].albedo_texture = texture_get(albedo_path.c_str());
//
//            // normal
//            std::string normal_path = dir + materials[i].normal_texname;
//            if (materials[i].normal_texname.empty()) {
//                // if no normal in mtl, use bump
//                normal_path += materials[i].bump_texname;
//            }
//            m_materials[i].normal_texture = texture_get(normal_path.c_str(), BuiltInTextureEnum::NORMAL);
//
//            // metallic
//            std::string metallic_path = dir + materials[i].metallic_texname;
//            m_materials[i].metallic_texture = texture_get(metallic_path.c_str(), BuiltInTextureEnum::BLACK);
//            m_materials[i].metallic_scale = materials[i].metallic;
//
//            // roughness
//            std::string roughness_path = dir + materials[i].roughness_texname;
//            m_materials[i].roughness_texture = texture_get(roughness_path.c_str(), BuiltInTextureEnum::WHITE);
//            m_materials[i].roughness_scale = materials[i].roughness;
//        }
//    }
//
//    // pre-pass to figure out how large each mesh of model is
//    for (u32 s = 0; s < shapes.size(); ++s) {
//        for (u32 f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
//            // material ids will be set to -1 if there aren't any materials
//            if (shapes[s].mesh.material_ids[f] < 0) {
//                shapes[s].mesh.material_ids[f] = 0;
//            }
//
//            u32 mesh_idx = shapes[s].mesh.material_ids[f];
//            u8 num_verts_per_face = shapes[s].mesh.num_face_vertices[f]; // this should always be 3
//
//            m_meshes[mesh_idx].num_vertices += num_verts_per_face;
//        }
//    }
//
//    // init meshes
//    for (u32 i = 0; i < m_numMeshes; ++i) {
//        // allocate memory for vertices
//        Mesh *mesh = &m_meshes[i];
//        mesh->vertices = (Vertex *) malloc(sizeof(Vertex) * mesh->num_vertices);
//        if (mesh->vertices == nullptr) {
//            core->fatal("Failed to allocate vertex memory for mesh");
//        }
//    }
//
//    // load vertex data for meshes, currently not an efficient way TODO: make more efficient
//    for (u32 m = 0; m < m_numMeshes; ++m) {
//        Mesh *mesh = &m_meshes[m];
//        Vertex *current = mesh->vertices;
//
//        for (u32 s = 0; s < shapes.size(); ++s) {
//            u32 index_offset = 0;
//            for (u32 f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
//                u32 mesh_idx = shapes[s].mesh.material_ids[f];
//                if (mesh_idx != m) continue;
//
//                int num_verts_per_face = shapes[s].mesh.num_face_vertices[f];
//                for (u32 v = 0; v < num_verts_per_face; ++v) {
//                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
//
//                    current->position = glm::vec3(
//                            attrib.vertices[3 * idx.vertex_index + 0],
//                            attrib.vertices[3 * idx.vertex_index + 1],
//                            attrib.vertices[3 * idx.vertex_index + 2]
//                    );
//                    current->normal = glm::normalize(glm::vec3(
//                            attrib.normals[3 * idx.normal_index + 0],
//                            attrib.normals[3 * idx.normal_index + 1],
//                            attrib.normals[3 * idx.normal_index + 2]
//                    ));
//                    current->uv = glm::vec2(
//                            attrib.texcoords[2 * idx.texcoord_index + 0],
//                            attrib.texcoords[2 * idx.texcoord_index + 1]
//                    );
//
//                    mesh->min = glm::min(mesh->min, current->position);
//                    mesh->max = glm::max(mesh->max, current->position);
//
//                    ++current;
//                }
//
//                if (num_verts_per_face != 3) {
//                    core->warn("num_verts_per_face is not 3");
//                    continue;
//                }
//
//                // calculate tangent and bi-tangent
//                Vertex *v0 = current - 3;
//                Vertex *v1 = current - 2;
//                Vertex *v2 = current - 1;
//
//                glm::vec3 delta_pos_1 = v1->position - v0->position;
//                glm::vec3 delta_pos_2 = v2->position - v0->position;
//
//                glm::vec2 delta_uv_1 = v1->uv - v0->uv;
//                glm::vec2 delta_uv_2 = v2->uv - v0->uv;
//
//                float r = 1.0f / (delta_uv_1.x * delta_uv_2.y - delta_uv_1.y * delta_uv_2.x);
//                glm::vec3 tangent = (delta_pos_1 * delta_uv_2.y - delta_pos_2 * delta_uv_1.y) * r;
//                glm::vec3 bi_tangent = (delta_pos_2 * delta_uv_1.x - delta_pos_1 * delta_uv_2.x) * r;
//
//                // set tangent and bi-tangent
//                for (Vertex *v : {v0, v1, v2}) {
//                    // make tangent orthogonal to normal
//                    v->tangent = glm::normalize(tangent - v->normal * glm::dot(v->normal, tangent));
//                    v->bi_tangent = bi_tangent;
//                }
//
//                index_offset += num_verts_per_face;
//            }
//        }
//    }
//
//    // setup vbos and vaos for meshes
//    for (u32 i = 0; i < m_numMeshes; ++i) {
//        Mesh *mesh = &m_meshes[i];
//        if (mesh->num_vertices == 0) {
//            fprintf(stderr, "[error] tried to init mesh with 0 vertices\n");
//        }
//
//        // create vao and vbo for rendering
//        glGenVertexArrays(1, &mesh->vao);
//        if (mesh->vao == 0) {
//            fprintf(stderr, "[error] failed to generate vao for mesh\n");
//        }
//        glBindVertexArray(mesh->vao);
//
//        glGenBuffers(1, &mesh->vbo);
//        if (mesh->vbo == 0) {
//            fprintf(stderr, "[error] failed to generate vbo for mesh\n");
//        }
//        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
//        glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);
//
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));
//
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));
//
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, uv));
//
//        glEnableVertexAttribArray(3);
//        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, tangent));
//
//        glEnableVertexAttribArray(4);
//        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, bi_tangent));
//
//        glBindVertexArray(0);
//    }
//}

void Model::init(const std::string &path) {
    size_t slashIdx = path.rfind('/');
    if (slashIdx == std::string::npos) {
        core->fatal("Failed to find directory for model path: '" + path + "'");
    }

    std::string dir = path.substr(0, slashIdx);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), dir.c_str(), true);
    if (!warn.empty()) {
        core->warn(warn);
    }
    if (!err.empty()) {
        core->fatal(err);
    }
    if (!result) {
        core->fatal("Failed to load OBJ '" + std::string(path) + "'");
    }

    bool hasNormals = !attrib.normals.empty();

    // Append default material
    materials.emplace_back();

    // Each mesh has exactly one material
    u32 numMeshes = materials.size();
    meshes.resize(numMeshes);

    // Set materials
    for (u32 i = 0; i < numMeshes; ++i) {
        meshes[i].material.metallic_scale = materials[i].metallic;
        meshes[i].material.roughness_scale = materials[i].roughness;

        // TODO: textures
    }

    for (auto &shape : shapes) {
        // iterate over faces
        for (u32 f = 0; f < shape.mesh.indices.size() / 3; ++f) {
            u32 matId = shape.mesh.material_ids[f];

            // set material to default material if invalid
            if (matId < 0 || matId >= materials.size()) {
                matId = materials.size() - 1;
            }

            // acorn meshes are associated w/ materials
            auto &vertices = meshes[matId].vertices;

            // add vertices to correct mesh
            for (u32 v = 0; v < 3; ++v) {
                tinyobj::index_t idx = shape.mesh.indices[3 * f + v];

                vertices.emplace_back();

                // set position
                vertices.back().position = glm::vec4(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2],
                        0
                );

                // set normal
                if (hasNormals) {
                    vertices.back().normal = glm::vec4(
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2],
                            0
                    );
                }

                vertices.back().uv = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                );

                meshes[matId].min = glm::min(meshes[matId].min, vertices.back().position);
                meshes[matId].max = glm::max(meshes[matId].max, vertices.back().position);
            }

            Vertex &v1 = vertices[vertices.size() - 3];
            Vertex &v2 = vertices[vertices.size() - 2];
            Vertex &v3 = vertices[vertices.size() - 1];

            // calculate normals if missing
            if (!hasNormals) {
                glm::vec3 u = v2.position - v1.position;
                glm::vec3 v = v3.position - v1.position;

                glm::vec4 normal = glm::vec4(glm::cross(u, v), 0);

                v1.normal = normal;
                v2.normal = normal;
                v3.normal = normal;
            }

            // calculate tangent and bi-tangent
            glm::vec3 delta_pos_1 = v2.position - v1.position;
            glm::vec3 delta_pos_2 = v3.position - v1.position;

            glm::vec2 delta_uv_1 = v2.uv - v1.uv;
            glm::vec2 delta_uv_2 = v3.uv - v1.uv;

            float r = 1.0f / (delta_uv_1.x * delta_uv_2.y - delta_uv_1.y * delta_uv_2.x);
            glm::vec3 tangent = (delta_pos_1 * delta_uv_2.y - delta_pos_2 * delta_uv_1.y) * r;
            glm::vec3 bi_tangent = (delta_pos_2 * delta_uv_1.x - delta_pos_1 * delta_uv_2.x) * r;

            // make tangent orthogonal to normal
            v1.tangent = glm::normalize(tangent - v1.normal * glm::dot(v1.normal, tangent));
            v1.bi_tangent = bi_tangent;

            v2.tangent = glm::normalize(tangent - v2.normal * glm::dot(v2.normal, tangent));
            v2.bi_tangent = bi_tangent;

            v3.tangent = glm::normalize(tangent - v3.normal * glm::dot(v3.normal, tangent));
            v3.bi_tangent = bi_tangent;
        }
    }

    for (auto &mesh : meshes) {
        mesh.init();
    }
}

void Model::destroy() {
    for (auto &mesh : meshes) {
        mesh.destroy();
    }
}
