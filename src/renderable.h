#ifndef ACORN_RENDERABLE_H
#define ACORN_RENDERABLE_H

#include "types.h"
#include "transform.h"
#include "model.h"

struct Renderable {
    Transform transform;
    Model *model = nullptr;
};

#endif //ACORN_RENDERABLE_H
