//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_TRANSFORM_H
#define NITRONIC_TRANSFORM_H
#include "core/Macros.h"
#include "glm/vec3.hpp"

NAMESPACE {

    struct Transform {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };

}

#endif //NITRONIC_TRANSFORM_H
