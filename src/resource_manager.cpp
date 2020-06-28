#include "resource_manager.h"
#include "core.h"

void ResourceManager::init() {
//    stbi_set_flip_vertically_on_load(1);
}

void ResourceManager::destroy() {
    for (auto &model : m_models) {
        model.second->destroy();
        delete model.second;
    }

//    for (auto &texture : m_textures) {
//        texture.second.destroy();
//    }
}

Model *ResourceManager::getModel(const std::string &path) {
    // TODO: I don't like the new, init, destroy, delete cycle - maybe move to constructors that fail for most things?

    // See if model is already loaded
    auto it = m_models.find(path);
    if (it != m_models.end()) {
        return it->second;
    }

    // Try to load model
    Model *model = new Model();
    model->init(path);
    m_models.emplace(path, model);
    return model;
}
