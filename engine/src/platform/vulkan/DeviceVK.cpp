#include "DeviceVK.h"

#include <map>
#include <set>

#include "VkMacros.h"
#include "nvrhi/validation.h"

NAMESPACE {

    std::vector deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
        SwapChainSupportDetails details{};

        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);

        return details;
    }

    SwapChainSupportDetails DeviceDataVk::GetSwapChainSupport() const {
        return QuerySwapChainSupport(physicalDevice, surface);
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

    bool CheckDeviceExtensionSupport(const vk::PhysicalDevice device) {
        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool IsDeviceSuitable(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
        const QueueFamilyIndices indices = FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
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
        m_DeviceData = new DeviceDataVk();
        DEVICE_DATA->surface = RENDERER_DATA->surface;

        std::vector<vk::PhysicalDevice> devices = RENDERER_DATA->instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            throw std::runtime_error("No Vulkan-compatible GPUs found.");
        }

        std::multimap<int, vk::PhysicalDevice> candidates;

        for (const auto& device : devices) {
            if (!IsDeviceSuitable(device, RENDERER_DATA->surface))
                continue;
            int score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.empty())
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");

        if (candidates.rbegin()->first > 0) {
            DEVICE_DATA->physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");
        }

        std::cout << "Using device " << devices[0].getProperties().deviceName << std::endl;

        DEVICE_DATA->queueFamilyIndices = FindQueueFamilies(DEVICE_DATA->physicalDevice, RENDERER_DATA->surface);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = { DEVICE_DATA->queueFamilyIndices.graphicsFamily.value(), DEVICE_DATA->queueFamilyIndices.presentFamily.value() };
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};

        vk::PhysicalDeviceVulkan11Features vulkan11Features{};

        vk::PhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.timelineSemaphore = VK_TRUE;
        vulkan12Features.pNext = &vulkan11Features;

        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
        dynamicRenderingFeatures.pNext = &vulkan12Features;

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.pNext = &dynamicRenderingFeatures;

        DEVICE_DATA->logicalDevice = DEVICE_DATA->physicalDevice.createDevice(deviceCreateInfo);
        DEVICE_DATA->graphicsQueue = DEVICE_DATA->logicalDevice.getQueue(DEVICE_DATA->queueFamilyIndices.graphicsFamily.value(), 0);
        DEVICE_DATA->presentQueue = DEVICE_DATA->logicalDevice.getQueue(DEVICE_DATA->queueFamilyIndices.presentFamily.value(), 0);

        nvrhi::vulkan::DeviceDesc deviceDesc{};
        deviceDesc.errorCB = &NvrhiMessageCallback::GetInstance();
        deviceDesc.physicalDevice = DEVICE_DATA->physicalDevice;
        deviceDesc.device = DEVICE_DATA->logicalDevice;
        deviceDesc.graphicsQueue = DEVICE_DATA->graphicsQueue;
        deviceDesc.graphicsQueueIndex = DEVICE_DATA->queueFamilyIndices.graphicsFamily.value();
        deviceDesc.deviceExtensions = deviceExtensions.data();
        deviceDesc.numDeviceExtensions = std::size(deviceExtensions);

        nvrhi::DeviceHandle nvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

#if _DEBUG
        m_Device = nvrhi::validation::createValidationLayer(nvrhiDevice);
#else
        m_Device = nvrhiDevice;
#endif
    }

    void Device::DestroyDeviceVk()
    {
        m_Device = nullptr;
        DEVICE_DATA->logicalDevice.destroy();
    }

}
