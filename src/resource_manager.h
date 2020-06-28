#ifndef ACORN_RESOURCE_MANAGER_H
#define ACORN_RESOURCE_MANAGER_H

#include "types.h"
#include "model.h"
//#include "mesh.h"
#include "texture.h"
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    void init();

    void destroy();

    Model *getModel(const std::string &path);

    //    void loadTextureFromPNG(const char *texture_path, TextureTypeEnum type);

private:
    std::unordered_map<std::string, Model *> m_models;
//    std::unordered_map<std::string, Texture*> m_textures;
};

#endif //ACORN_RESOURCE_MANAGER_H
