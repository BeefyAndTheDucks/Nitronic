//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_TRANSFORM_H
#define NITRONIC_TRANSFORM_H
#include <unordered_set>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "core/Macros.h"

NAMESPACE {

    class Transform {
    public:
        glm::vec3 position = glm::vec3(0.f);
        glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
        glm::vec3 scale = glm::vec3(1.f);

        bool isStatic;

    public:
        void SetParent(Transform* newParent);
        void ClearParent() { SetParent(nullptr); }
        [[nodiscard]] Transform* GetParent() const { return m_Parent; }

        [[nodiscard]] std::unordered_set<Transform*>& GetChildren() { return m_Children; }

        [[nodiscard]] glm::mat4 GetMatrix() const;

    private:
        void AddChild(Transform* child);
        void RemoveChild(Transform* child);

    private:
        Transform* m_Parent = nullptr;
        std::unordered_set<Transform*> m_Children;
    };

}

#endif //NITRONIC_TRANSFORM_H
