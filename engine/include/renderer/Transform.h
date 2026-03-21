//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_TRANSFORM_H
#define NITRONIC_TRANSFORM_H
#include "core/Macros.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

NAMESPACE {

    struct Transform {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        glm::mat4 GetMatrix() const;
    };

}

#endif //NITRONIC_TRANSFORM_H
