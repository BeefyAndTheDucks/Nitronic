//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_CAMERA_H
#define NITRONIC_CAMERA_H
#include "engine/Transform.h"
#include "core/Macros.h"

NAMESPACE {

    struct Camera {
        float fov = 45;

        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        Transform transform{};

        [[nodiscard]] glm::mat4 GetViewMatrix() const;
        [[nodiscard]] glm::mat4 GetProjectionMatrix(float aspect) const;
        [[nodiscard]] glm::mat4 GetViewProjectionMatrix(float aspect) const;
    };

}

#endif //NITRONIC_CAMERA_H