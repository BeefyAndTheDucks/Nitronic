//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_CAMERA_H
#define NITRONIC_CAMERA_H
#include "Transform.h"
#include "core/Macros.h"

NAMESPACE {

    struct Camera {
        Transform transform;
        float fov;

        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(float aspect) const;
        glm::mat4 GetViewProjectionMatrix(float aspect) const;
    };

}

#endif //NITRONIC_CAMERA_H