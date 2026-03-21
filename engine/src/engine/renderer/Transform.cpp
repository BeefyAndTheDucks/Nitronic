//
// Created by simon on 21/03/2026.
//

#include "renderer/Transform.h"

#include "glm/gtc/quaternion.hpp"

NAMESPACE {

    glm::mat4 Transform::GetMatrix() const {
        return glm::translate(glm::identity<glm::mat4>(), position)
            * glm::mat4_cast(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

}
