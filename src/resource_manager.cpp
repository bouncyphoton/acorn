#include "resource_manager.h"
#include "core.h"
#include "utils.h"
#include <stb_image.h>

ResourceManager::ResourceManager() {
    core->debug("ResourceManager::ResourceManager()");
    init();
}

ResourceManager::~ResourceManager() {
    core->debug("ResourceManager::~ResourceManager()");
    destroy();
}

Model *ResourceManager::getModel(const std::string &path) {
    // See if model is already loaded
    auto it = m_models.find(path);
    if (it != m_models.end()) {
        return it->second;
    }

    // Try to load model
    Model *model = new Model(path);
    m_models.emplace(path, model);
    return model;
}

Texture *ResourceManager::getTexture(const std::string &path) {
    // See if texture is already loaded
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Try to load texture
    core->info("Loading texture '" + path + "'");
    Texture *texture = new Texture();
    texture->init(path);
    m_textures.emplace(path, texture);
    return texture;
}

Texture *ResourceManager::getBuiltInTexture(BuiltInTextureEnum tex) {
    switch (tex) {
        case BuiltInTextureEnum::BLACK:
            return m_textureBlack;
        case BuiltInTextureEnum::WHITE:
            return m_textureWhite;
        case BuiltInTextureEnum::NORMAL:
            return m_textureNormal;
        default:
            core->warn("Unhandled case for getting a built in texture: " +
                       std::to_string(static_cast<int>(tex)));
        case BuiltInTextureEnum::MISSING:
            return m_textureMissing;
    }
}

Model *ResourceManager::getBuiltInModel(BuiltInModelEnum model) {
    switch (model) {
        case BuiltInModelEnum::PLANE:
            return m_modelPlane;
        default:
            core->warn("Unhandled case for getting a built in mode: "
                       + std::to_string(static_cast<int>(model)));
            return nullptr;
    }
}

void ResourceManager::init() {
    stbi_set_flip_vertically_on_load(1);

    // Load built-in textures
    u8 black[4] = {0, 0, 0, 255};
    m_textureBlack = new Texture();
    m_textureBlack->init2D(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, black, GL_RGBA);

    u8 white[4] = {255, 255, 255, 255};
    m_textureWhite = new Texture();
    m_textureWhite->init2D(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, white, GL_RGBA);

    u8 normal[4] = {127, 127, 255, 255};
    m_textureNormal = new Texture();
    m_textureNormal->init2D(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, normal, GL_RGBA);

    u8 missing[4 * 4] = {255, 0, 255, 255,
                         0, 0, 0, 255,
                         255, 0, 255, 255,
                         0, 0, 0, 255};
    m_textureMissing = new Texture();
    m_textureMissing->init2D(GL_RGBA, 2, 2, GL_UNSIGNED_BYTE, missing, GL_RGBA);

    // Load built-in models
    glm::vec3 norm = glm::vec3(0, 1, 0);
    Vertex v1 = {
            glm::vec3(-1, 0, -1), // position
            norm, // normal
            glm::vec2(0, 0), // uv
            glm::vec3(0), glm::vec3(0) // tangent and bi-tangent will be calculated later
    };
    Vertex v2 = {
            glm::vec3(1, 0, -1), // position
            norm, // normal
            glm::vec2(1, 0), // uv
            glm::vec3(0), glm::vec3(0) // tangent and bi-tangent will be calculated later
    };
    Vertex v3 = {
            glm::vec3(1, 0, 1), // position
            norm, // normal
            glm::vec2(1, 1), // uv
            glm::vec3(0), glm::vec3(0) // tangent and bi-tangent will be calculated later
    };
    Vertex v4 = {
            glm::vec3(-1, 0, 1), // position
            norm, // normal
            glm::vec2(0, 1), // uv
            glm::vec3(0), glm::vec3(0) // tangent and bi-tangent will be calculated later
    };

    std::vector<Vertex> vertices = {v1, v2, v3, v1, v3, v4};
    utils::calculate_tangent_and_bi_tangent(vertices[0],
                                            vertices[1],
                                            vertices[2]);
    utils::calculate_tangent_and_bi_tangent(vertices[3],
                                            vertices[4],
                                            vertices[5]);

    Material material;
    material.albedoTexture = m_textureWhite->id;
    material.normalTexture = m_textureNormal->id;
    material.metallicTexture = m_textureWhite->id;
    material.metallicScale = 0;
    material.roughnessTexture = m_textureWhite->id;
    material.roughnessScale = 1;

    std::vector<Mesh> m;
    m.emplace_back(vertices, material);
    m_modelPlane = new Model(std::move(m));
}

void ResourceManager::destroy() {
    for (auto &model : m_models) {
        delete model.second;
    }

    for (auto &texture : m_textures) {
        texture.second->destroy();
        delete texture.second;
    }

    // TODO: I don't like the new, init, destroy, delete cycle - maybe move to constructors that fail for most things?
    m_textureBlack->destroy();
    delete m_textureBlack;
    m_textureWhite->destroy();
    delete m_textureWhite;
    m_textureNormal->destroy();
    delete m_textureNormal;
    m_textureMissing->destroy();
    delete m_textureMissing;

    delete m_modelPlane;
}
