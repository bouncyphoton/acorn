#include "model.h"
#include "texture.h"
#include "log.h"
#include "utils.h"

// TODO: move away from OBJs to binary blobs for meshes
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#undef min
#undef max

Model::Model(const std::string &path) {
    Log::debug("Model::Model(%s)", path.c_str());
    init(path);
}

Model::Model(std::vector<Mesh> &&meshes)
    : m_meshes(std::move(meshes)) {
    Log::debug("Model::Model(%d meshes)", meshes.size());
}

Model::~Model() {
    Log::debug("Model::~Model()");
}

void Model::init(const std::string &path) {
    size_t slashIdx = path.rfind('/');
    if (slashIdx == std::string::npos) {
        Log::fatal("Failed to find directory for model path: '%s'", path.c_str());
    }

    std::string dir = path.substr(0, slashIdx);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> tinyObjMaterials;
    std::string warn, err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &tinyObjMaterials, &warn, &err, path.c_str(), dir.c_str(), true);
    if (!warn.empty()) {
        Log::warn(warn.c_str());
    }
    if (!err.empty()) {
        Log::fatal(err.c_str());
    }
    if (!result) {
        Log::fatal("Failed to load OBJ '%s'", path.c_str());
    }

    bool hasNormals = !attrib.normals.empty();

    // Append default material
    tinyObjMaterials.emplace_back();

    // Each mesh has exactly one material
    u32 numMeshes = tinyObjMaterials.size();
    std::vector<Material> meshMaterials;
    meshMaterials.resize(numMeshes);
    std::vector<std::vector<Vertex>> meshVertices(numMeshes);
    meshVertices.resize(numMeshes);

    // Set materials
    for (u32 i = 0; i < numMeshes; ++i) {
        // Diffuse texture
        if (!tinyObjMaterials[i].diffuse_texname.empty()) {
            meshMaterials[i].albedoTexture = core->resourceManager.getTexture(
                                                 dir + "/" + tinyObjMaterials[i].diffuse_texname)->getId();
        } else {
            meshMaterials[i].albedoTexture = core->resourceManager.getBuiltInTexture(
                                                 BuiltInTextureEnum::MISSING)->getId();
        }

        // Normal texture
        if (!tinyObjMaterials[i].normal_texname.empty()) {
            meshMaterials[i].normalTexture = core->resourceManager.getTexture(
                                                 dir + "/" + tinyObjMaterials[i].normal_texname)->getId();
        } else if (!tinyObjMaterials[i].bump_texname.empty()) {
            meshMaterials[i].normalTexture = core->resourceManager.getTexture(
                                                 dir + "/" + tinyObjMaterials[i].bump_texname)->getId();
        }
        {
            meshMaterials[i].normalTexture = core->resourceManager.getBuiltInTexture(
                                                 BuiltInTextureEnum::NORMAL)->getId();
        }

        // Metallic texture
        if (!tinyObjMaterials[i].metallic_texname.empty()) {
            meshMaterials[i].metallicTexture = core->resourceManager.getTexture(
                                                   dir + "/" + tinyObjMaterials[i].metallic_texname)->getId();
        } else {
            meshMaterials[i].metallicTexture = core->resourceManager.getBuiltInTexture(
                                                   BuiltInTextureEnum::WHITE)->getId();
        }
        meshMaterials[i].metallicScale = tinyObjMaterials[i].metallic;

        // Roughness texture
        if (!tinyObjMaterials[i].roughness_texname.empty()) {
            meshMaterials[i].roughnessTexture = core->resourceManager.getTexture(
                                                    dir + "/" + tinyObjMaterials[i].roughness_texname)->getId();
        } else {
            meshMaterials[i].roughnessTexture = core->resourceManager.getBuiltInTexture(
                                                    BuiltInTextureEnum::WHITE)->getId();
        }
        meshMaterials[i].roughnessScale = tinyObjMaterials[i].roughness;
    }

    for (auto &shape : shapes) {
        // iterate over faces
        for (u32 f = 0; f < shape.mesh.indices.size() / 3; ++f) {
            u32 matId = shape.mesh.material_ids[f];

            // set material to default material if invalid
            if (matId < 0 || matId >= tinyObjMaterials.size()) {
                matId = tinyObjMaterials.size() - 1;
            }

            // acorn meshes are associated w/ materials
            auto &vertices = meshVertices[matId];

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

    // Only add meshes with vertices
    for (u32 i = 0; i < numMeshes; ++i) {
        if (!meshVertices[i].empty()) {
            m_meshes.emplace_back(meshVertices[i], meshMaterials[i]);
        }
    }
}
