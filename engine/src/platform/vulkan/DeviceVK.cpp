#include "DeviceVK.h"

#include <map>

#include "VkMacros.h"

NAMESPACE {

    void Device::CreateDeviceDataVk(RendererData* rendererData) {
        m_DeviceData = new DeviceDataVk();
        m_DeviceData->m_RendererData = rendererData;
    }

    QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice device) {
        QueueFamilyIndices indices{};

        const std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlags::BitsType::eGraphics)
                indices.graphicsFamily = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    bool IsDeviceSuitable(const vk::PhysicalDevice device) {
        const QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.IsComplete();
    }

    int RateDeviceSuitability(const vk::PhysicalDevice device) {
        const vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

        int score = 0;

        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }

        score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

        return score;
    }

    void Device::CreateDeviceVk()
    {
        std::vector<vk::PhysicalDevice> devices = RENDERER_DATA_FROM_DEVICE->m_Instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            throw std::runtime_error("No Vulkan-compatible GPUs found.");
        }

        std::multimap<int, vk::PhysicalDevice> candidates;

        for (const auto& device : devices) {
            if (!IsDeviceSuitable(device))
                continue;
            int score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            DEVICE_DATA->m_PhysicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");
        }

        if (DEVICE_DATA->m_PhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");
        }

        std::cout << "Using device " << devices[0].getProperties().deviceName << std::endl;

        DEVICE_DATA->m_QueueFamilyIndices = FindQueueFamilies(DEVICE_DATA->m_PhysicalDevice);

        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = DEVICE_DATA->m_QueueFamilyIndices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        constexpr float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        vk::PhysicalDeviceFeatures deviceFeatures{};

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;

        DEVICE_DATA->m_LogicalDevice = DEVICE_DATA->m_PhysicalDevice.createDevice(deviceCreateInfo);
    }

    void Device::DestroyDeviceVk()
    {
        DEVICE_DATA->m_LogicalDevice.destroy();
    }

}
