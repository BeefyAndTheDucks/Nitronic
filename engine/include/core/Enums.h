//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_ENUMS_H
#define NITRONIC_ENUMS_H

enum RenderingBackend {
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
        case Vulkan: return "Vulkan";
#endif
#ifdef NITRONIC_WITH_DX11
        case DirectX11: return "DirectX11";
#endif
#ifdef NITRONIC_WITH_DX12
        case DirectX12: return "DirectX12";
#endif
        default: return "Unknown";
    }
}

#endif //NITRONIC_ENUMS_H