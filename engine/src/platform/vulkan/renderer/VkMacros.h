//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_VKMACROS_H
#define NITRONIC_VKMACROS_H
#include "RendererVK.h"
#include "DeviceVK.h"
#include "ImGuiRendererVk.h"
#include "core/Macros.h"

#include <iostream>

NAMESPACE {

    inline RendererDataVk* GetRendererDataChecked(RendererData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<RendererDataVk*>(base);
        if (!ptr) {
            std::cerr << "Failed to get RendererData at " << file << ":" << line << std::endl;
            std::abort();
        }
        return ptr;
    }

    inline DeviceDataVk* GetDeviceDataChecked(DeviceData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<DeviceDataVk*>(base);
        if (!ptr) {
            std::cerr << "Failed to get DeviceData at " << file << ":" << line << std::endl;
            std::abort();
        }
        return ptr;
    }

    inline ImGuiRendererDataVk* GetImGuiRendererDataChecked(ImGuiRendererData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<ImGuiRendererDataVk*>(base);
        if (!ptr) {
            std::cerr << "Failed to get ImGuiRendererData at " << file << ":" << line << std::endl;
            std::abort();
        }
        return ptr;
    }

#define RENDERER_DATA GetRendererDataChecked(m_RendererData, __FILE__, __LINE__)
#define DEVICE_DATA GetDeviceDataChecked(m_DeviceData, __FILE__, __LINE__)
#define IMGUI_RENDERER_DATA GetImGuiRendererDataChecked(m_ImGuiRendererData, __FILE__, __LINE__)

#define RENDERER_DATA_FROM_BASE(base) GetRendererDataChecked(base, __FILE__, __LINE__)
#define DEVICE_DATA_FROM_BASE(base) GetDeviceDataChecked(base, __FILE__, __LINE__)
#define IMGUI_RENDERER_DATA_FROM_BASE(base) GetImGuiRendererDataChecked(base, __FILE__, __LINE__)

}

#endif //NITRONIC_VKMACROS_H