#ifndef ACORN_TRANSFORM_H
#define ACORN_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 scale = glm::vec3(1);
};

inline glm::mat4 transform_to_matrix(Transform *transform) {
    if (transform == nullptr) return glm::mat4(1);

    glm::mat4 t = glm::translate(glm::mat4(1), transform->position);
    glm::mat4 r = glm::mat4_cast(transform->orientation);
    glm::mat4 s = glm::scale(glm::mat4(1), transform->scale);

    return t * r * s;
}

#endif //ACORN_TRANSFORM_H
