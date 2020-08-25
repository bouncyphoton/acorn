#ifndef ACORN_MODEL_H
#define ACORN_MODEL_H

#include "mesh.h"
#include <string>
#include <vector>

class Model {
public:
    Model(const std::string &path);
    Model(std::vector<Mesh> &&meshes);
    ~Model();

    const std::vector<Mesh> &getMeshes() const {
        return m_meshes;
    }

private:
    void init(const std::string &path);

    std::vector<Mesh> m_meshes;
};

#endif //ACORN_MODEL_H
