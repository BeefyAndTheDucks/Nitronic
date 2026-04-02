//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_ENUMS_HPP
#define NITRONIC_ENUMS_HPP
#include "Macros.hpp"

NAMESPACE {

#if !defined(NITRONIC_WITH_VULKAN) && !defined(NITRONIC_WITH_DX11) && !defined(NITRONIC_WITH_DX12)
#error "At least one rendering backend must be enabled (NITRONIC_WITH_VULKAN, NITRONIC_WITH_DX11, or NITRONIC_WITH_DX12)"
#endif

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

}

#endif //NITRONIC_ENUMS_HPP