//
// Created by simon on 21/03/2026.
//

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include "renderer/Camera.hpp"

#include "glm/gtc/quaternion.hpp"

NAMESPACE {

    glm::mat4 Camera::GetViewMatrix() const {
        const glm::quat inverseRotation = glm::conjugate(transform.rotation);
        glm::mat4 rotationMatrix = glm::mat4_cast(inverseRotation);
        const glm::vec3 inversePosition = inverseRotation * -transform.position;
        rotationMatrix[3] = glm::vec4(inversePosition, 1.0f);

        return rotationMatrix;
    }

    glm::mat4 Camera::GetProjectionMatrix(const float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    glm::mat4 Camera::GetViewProjectionMatrix(const float aspect) const {
        return GetProjectionMatrix(aspect) * GetViewMatrix();
    }
}
