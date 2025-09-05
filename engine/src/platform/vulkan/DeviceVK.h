//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_DEVICEVK_H
#define NITRONIC_DEVICEVK_H

#include <optional>

#include "renderer/Device.h"
#include "RendererVK.h"

NAMESPACE {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool IsComplete() const {
            return graphicsFamily.has_value();
        }
    };

    struct DeviceDataVk : DeviceData {
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;
        QueueFamilyIndices m_QueueFamilyIndices;
    };

}

#endif //NITRONIC_DEVICEVK_H