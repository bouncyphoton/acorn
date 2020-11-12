#ifndef ACORN_TRANSFORM_H
#define ACORN_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    Transform(glm::vec3 position = glm::vec3(0),
              glm::quat orientation = glm::identity<glm::quat>(),
              glm::vec3 scale = glm::vec3(1))
        : position(position), orientation(orientation), scale(scale) {}

    glm::mat4 toMatrix() const {
        glm::mat4 t = glm::translate(glm::mat4(1), position);
        glm::mat4 r = glm::mat4_cast(orientation);
        glm::mat4 s = glm::scale(glm::mat4(1), scale);

        return t * r * s;
    }

    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
};

#endif //ACORN_TRANSFORM_H
