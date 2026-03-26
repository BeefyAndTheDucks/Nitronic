//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_VKMACROS_H
#define NITRONIC_VKMACROS_H
#include "RendererVK.h"
#include "DeviceVK.h"
#include "ImGuiRendererVk.h"
#include "core/Macros.h"

NAMESPACE {

    inline RendererDataVk* GetRendererDataChecked(RendererData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<RendererDataVk*>(base);
        ENGINE_ASSERT(ptr, "Failed to get RendererData at {}:{}", file, line);
        return ptr;
    }

    inline DeviceDataVk* GetDeviceDataChecked(DeviceData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<DeviceDataVk*>(base);
        ENGINE_ASSERT(ptr, "Failed to get DeviceData at {}:{}", file, line);
        return ptr;
    }

    inline ImGuiRendererDataVk* GetImGuiRendererDataChecked(ImGuiRendererData* base, const char* file, const int line) {
        auto* ptr = dynamic_cast<ImGuiRendererDataVk*>(base);
        ENGINE_ASSERT(ptr, "Failed to get ImGuiRendererData at {}:{}", file, line);
        return ptr;
    }

#define RENDERER_DATA_OWNED GetRendererDataChecked(m_RendererData.get(), __FILE__, __LINE__)
#define DEVICE_DATA_OWNED GetDeviceDataChecked(m_DeviceData.get(), __FILE__, __LINE__)
#define IMGUI_RENDERER_DATA_OWNED GetImGuiRendererDataChecked(m_ImGuiRendererData.get(), __FILE__, __LINE__)

#define RENDERER_DATA GetRendererDataChecked(m_RendererData, __FILE__, __LINE__)
#define DEVICE_DATA GetDeviceDataChecked(m_DeviceData, __FILE__, __LINE__)
#define IMGUI_RENDERER_DATA GetImGuiRendererDataChecked(m_ImGuiRendererData, __FILE__, __LINE__)

#define RENDERER_DATA_FROM_BASE(base) GetRendererDataChecked(base, __FILE__, __LINE__)
#define DEVICE_DATA_FROM_BASE(base) GetDeviceDataChecked(base, __FILE__, __LINE__)
#define IMGUI_RENDERER_DATA_FROM_BASE(base) GetImGuiRendererDataChecked(base, __FILE__, __LINE__)

}

#endif //NITRONIC_VKMACROS_H