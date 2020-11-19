#include "model.h"
#include "texture.h"
#include "log.h"
#include "utils.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

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
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_GenNormals |
                                             aiProcess_GenUVCoords);

    if (!scene) {
        Log::fatal("Failed to load model: %s", importer.GetErrorString());
    }

    std::string dir = path.substr(0, path.find_last_of('/') + 1);

    // Process scene
    for (u32 m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh *mesh = scene->mMeshes[m];

        // TODO: index buffer

        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumFaces * 3);
        for (u32 f = 0; f < mesh->mNumFaces; ++f) {
            aiFace face = mesh->mFaces[f];
            for (u32 i = 0; i < face.mNumIndices; ++i) {
                u32 v = face.mIndices[i];
                Vertex vertex = {};

                vertex.position = {
                    mesh->mVertices[v].x,
                    mesh->mVertices[v].y,
                    mesh->mVertices[v].z
                };

                vertex.normal = {
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                };

                vertex.uv = {
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                };

                vertex.tangent = {
                    mesh->mTangents[v].x,
                    mesh->mTangents[v].y,
                    mesh->mTangents[v].z
                };

                vertex.biTangent = {
                    mesh->mBitangents[v].x,
                    mesh->mBitangents[v].y,
                    mesh->mBitangents[v].z
                };

                vertices.emplace_back(vertex);
            }
        }

        // Default material
        Material material;
        material.albedoTexture = core->resourceManager.getBuiltInTexture(BuiltInTextureEnum::WHITE);
        material.albedoScale = glm::vec3(1);
        material.normalTexture = core->resourceManager.getBuiltInTexture(BuiltInTextureEnum::NORMAL);
        material.metallicTexture = core->resourceManager.getBuiltInTexture(BuiltInTextureEnum::WHITE);
        material.metallicScale = 1.0f;
        material.roughnessTexture = core->resourceManager.getBuiltInTexture(BuiltInTextureEnum::WHITE);
        material.roughnessScale = 1.0f;

        // Load material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];

            auto loadTexture = [&](aiTextureType type, Texture **location) {
                if (aiMat->GetTextureCount(type) > 0) {
                    aiString texRelativePath;
                    aiMat->GetTexture(type, 0, &texRelativePath);
                    std::string texPath = dir + std::string(texRelativePath.C_Str());
                    std::replace(texPath.begin(), texPath.end(), '\\', '/');
                    *location = core->resourceManager.getTexture(texPath);
                }
            };

            loadTexture(aiTextureType_DIFFUSE, &material.albedoTexture);
            loadTexture(aiTextureType_NORMALS, &material.normalTexture);
            loadTexture(aiTextureType_METALNESS, &material.metallicTexture);
            loadTexture(aiTextureType_DIFFUSE_ROUGHNESS, &material.roughnessTexture);

            // Special case where metallic and roughness are in same texture
            aiString metalRoughPath;
            if (aiMat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE,
                                  &metalRoughPath) == aiReturn_SUCCESS) {
                std::string texPath = dir + std::string(metalRoughPath.C_Str());
                std::replace(texPath.begin(), texPath.end(), '\\', '/');

                // Seems that usually this is occlusion, roughness, metallic (RGB respectively)?
                core->resourceManager.getTextureSplitComponents(texPath, nullptr, &material.roughnessTexture,
                                                                &material.metallicTexture, nullptr);
            }

            aiColor3D aiAlbedoScale;
            aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, aiAlbedoScale);
            material.albedoScale = glm::vec3(aiAlbedoScale.r, aiAlbedoScale.g, aiAlbedoScale.b);

            aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, material.metallicScale);
            aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, material.roughnessScale);
        }

        m_meshes.emplace_back(vertices, material);
    }
}
