#include "DeviceVK.h"

#include <map>
#include <set>

#include "VkMacros.h"

NAMESPACE {

    void Device::CreateDeviceDataVk(RendererData* rendererData) {
        m_DeviceData = new DeviceDataVk();
        m_DeviceData->m_RendererData = rendererData;
    }

    QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
        QueueFamilyIndices indices{};

        const std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlags::BitsType::eGraphics)
                indices.graphicsFamily = i;

            if (device.getSurfaceSupportKHR(i, surface))
                indices.presentFamily = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    bool IsDeviceSuitable(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
        const QueueFamilyIndices indices = FindQueueFamilies(device, surface);

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
            if (!IsDeviceSuitable(device, RENDERER_DATA_FROM_DEVICE->m_Surface))
                continue;
            int score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.empty())
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");

        if (candidates.rbegin()->first > 0) {
            DEVICE_DATA->m_PhysicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");
        }

        std::cout << "Using device " << devices[0].getProperties().deviceName << std::endl;

        DEVICE_DATA->m_QueueFamilyIndices = FindQueueFamilies(DEVICE_DATA->m_PhysicalDevice, RENDERER_DATA_FROM_DEVICE->m_Surface);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = { DEVICE_DATA->m_QueueFamilyIndices.graphicsFamily.value(), DEVICE_DATA->m_QueueFamilyIndices.presentFamily.value() };
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;

        DEVICE_DATA->m_LogicalDevice = DEVICE_DATA->m_PhysicalDevice.createDevice(deviceCreateInfo);
        DEVICE_DATA->m_GraphicsQueue = DEVICE_DATA->m_LogicalDevice.getQueue(DEVICE_DATA->m_QueueFamilyIndices.graphicsFamily.value(), 0);
        DEVICE_DATA->m_PresentQueue = DEVICE_DATA->m_LogicalDevice.getQueue(DEVICE_DATA->m_QueueFamilyIndices.presentFamily.value(), 0);
    }

    void Device::DestroyDeviceVk()
    {
        DEVICE_DATA->m_LogicalDevice.destroy();
    }

}
