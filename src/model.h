#ifndef ACORN_MODEL_H
#define ACORN_MODEL_H

#include "mesh.h"
#include <string>
#include <vector>

class Model {
public:
    void init(const std::string &path);

    void destroy();

    std::vector<Mesh> meshes;
};

#endif //ACORN_MODEL_H
