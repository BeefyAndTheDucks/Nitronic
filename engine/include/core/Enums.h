//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_ENUMS_H
#define NITRONIC_ENUMS_H
#include "nvrhi/nvrhi.h"

enum class RenderingBackend {
#ifdef NITRONIC_WITH_VULKAN
    Vulkan,
#endif
#ifdef NITRONIC_WITH_DX11
    DirectX11,
#endif
#ifdef NITRONIC_WITH_DX12
    DirectX12,
#endif
};

inline const char* RenderingBackendToString(const RenderingBackend backend) {
    switch (backend) {
#ifdef NITRONIC_WITH_VULKAN
        case RenderingBackend::Vulkan: return "Vulkan";
#endif
#ifdef NITRONIC_WITH_DX11
        case RenderingBackend::DirectX11: return "DirectX11";
#endif
#ifdef NITRONIC_WITH_DX12
        case RenderingBackend::DirectX12: return "DirectX12";
#endif
        default: return "Unknown";
    }
}

enum class ShaderType {
    Fragment,
    Vertex
};

inline const char* ShaderTypeToString(const ShaderType type) {
    switch (type) {
        case ShaderType::Fragment: return "Fragment";
        case ShaderType::Vertex: return "Vertex";
        default: return "Unknown";
    }
}

constexpr nvrhi::ShaderType ShaderTypeToNvrhiShaderType(const ShaderType type) {
    switch (type) {
        case ShaderType::Fragment: return nvrhi::ShaderType::Pixel;
        case ShaderType::Vertex: return nvrhi::ShaderType::Vertex;
        default: return nvrhi::ShaderType::None;
    }
}

#endif //NITRONIC_ENUMS_H