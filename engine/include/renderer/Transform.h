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
        glm::vec3 position = glm::vec3(0.f);
        glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
        glm::vec3 scale = glm::vec3(1.f);

        [[nodiscard]] glm::mat4 GetMatrix() const;
    };

}

#endif //NITRONIC_TRANSFORM_H
