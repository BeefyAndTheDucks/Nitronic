//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_RENDERERTYPES_HPP
#define NITRONIC_RENDERERTYPES_HPP
#include "core/Macros.hpp"
#include <glm/glm.hpp>

NAMESPACE {

    struct RendererData {
        virtual ~RendererData() = default;
    };

    struct DeviceData {
        virtual ~DeviceData() = default;
    };

    struct ImGuiRendererData {
        virtual ~ImGuiRendererData() = default;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

}

#endif //NITRONIC_RENDERERTYPES_HPP