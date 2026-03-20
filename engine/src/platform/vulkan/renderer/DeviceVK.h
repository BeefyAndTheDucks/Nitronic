//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_DEVICEVK_H
#define NITRONIC_DEVICEVK_H

#include "VulkanInclude.h"

#include <optional>

#include "renderer/Device.h"

NAMESPACE {

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct DeviceDataVk : DeviceData {
        vk::PhysicalDevice physicalDevice;
        vk::Device logicalDevice;
        vk::SurfaceKHR surface;
        QueueFamilyIndices queueFamilyIndices;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
    public:
        [[nodiscard]] SwapChainSupportDetails GetSwapChainSupport() const;
    };

}

#endif //NITRONIC_DEVICEVK_H