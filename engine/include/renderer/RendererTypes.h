//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_RENDERERTYPES_H
#define NITRONIC_RENDERERTYPES_H
#include "core/Macros.h"
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

#endif //NITRONIC_RENDERERTYPES_H