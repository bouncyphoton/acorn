#include "model.h"
#include "texture.h"
#include "core.h"
#include "utils.h"

// TODO: move away from OBJs to binary blobs for meshes
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#undef min
#undef max

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
        // Diffuse texture
        if (!materials[i].diffuse_texname.empty()) {
            meshes[i].material.albedoTexture = core->resourceManager.getTexture(
                    dir + "/" + materials[i].diffuse_texname)->id;
        } else {
            meshes[i].material.albedoTexture = core->resourceManager.getBuiltInTexture(
                    BuiltInTextureEnum::MISSING)->id;
        }

        // Normal texture
        if (!materials[i].normal_texname.empty()) {
            meshes[i].material.normalTexture = core->resourceManager.getTexture(
                    dir + "/" + materials[i].normal_texname)->id;
        } else if (!materials[i].bump_texname.empty()) {
            meshes[i].material.normalTexture = core->resourceManager.getTexture(
                    dir + "/" + materials[i].bump_texname)->id;
        }
        {
            meshes[i].material.normalTexture = core->resourceManager.getBuiltInTexture(
                    BuiltInTextureEnum::NORMAL)->id;
        }

        // Metallic texture
        if (!materials[i].metallic_texname.empty()) {
            meshes[i].material.metallicTexture = core->resourceManager.getTexture(
                    dir + "/" + materials[i].metallic_texname)->id;
        } else {
            meshes[i].material.metallicTexture = core->resourceManager.getBuiltInTexture(
                    BuiltInTextureEnum::WHITE)->id;
        }
        meshes[i].material.metallicScale = materials[i].metallic;

        // Roughness texture
        if (!materials[i].roughness_texname.empty()) {
            meshes[i].material.roughnessTexture = core->resourceManager.getTexture(
                    dir + "/" + materials[i].roughness_texname)->id;
        } else {
            meshes[i].material.roughnessTexture = core->resourceManager.getBuiltInTexture(
                    BuiltInTextureEnum::WHITE)->id;
        }
        meshes[i].material.roughnessScale = materials[i].roughness;
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
                vertices.back().position = glm::vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                );

                // set normal
                if (hasNormals) {
                    vertices.back().normal = glm::vec3(
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]
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

                glm::vec3 normal = glm::vec3(glm::cross(u, v));

                v1.normal = normal;
                v2.normal = normal;
                v3.normal = normal;
            }

            // calculate tangent and bi-tangent
            utils::calculate_tangent_and_bi_tangent(v1, v2, v3);
        }
    }

    for (u32 i = 0; i < meshes.size();) {
        // Remove meshes with no vertices
        if (meshes[i].vertices.empty()) {
            meshes.erase(meshes.begin() + i);
        } else {
            meshes[i].init();
            ++i;
        }
    }
}

void Model::destroy() {
    for (auto &mesh : meshes) {
        mesh.destroy();
    }
}
