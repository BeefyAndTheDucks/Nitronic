//
// Created by simon on 21/03/2026.
//

#include "engine/Transform.h"

#include "engine/Log.h"
#include "glm/gtc/quaternion.hpp"

NAMESPACE {

    void Transform::SetParent(Transform *newParent) {
        if (m_Parent)
            m_Parent->RemoveChild(this);

        if (newParent)
            newParent->AddChild(this);

        m_Parent = newParent;
    }

    glm::mat4 Transform::GetMatrix() const {
        return glm::translate(glm::identity<glm::mat4>(), position)
            * glm::mat4_cast(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    void Transform::AddChild(Transform *child) {
        ENGINE_ASSERT(m_Children.insert(child).second, "Failed to add child to m_Children.");
    }

    void Transform::RemoveChild(Transform *child) {
        ENGINE_ASSERT(m_Children.erase(child), "Failed to add child to m_Children.");
    }
}
