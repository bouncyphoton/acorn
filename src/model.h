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

    std::vector<Mesh> meshes;
private:
    void init(const std::string &path);
};

#endif //ACORN_MODEL_H
