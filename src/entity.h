#ifndef ACORN_ENTITY_H
#define ACORN_ENTITY_H

#include "types.h"
#include "model.h"
#include "transform.h"

typedef u32 entityHandle_t;

struct Entity {
    Model *model = nullptr;
    Transform transform = {};
    bool active = true;
};

#endif //ACORN_ENTITY_H
