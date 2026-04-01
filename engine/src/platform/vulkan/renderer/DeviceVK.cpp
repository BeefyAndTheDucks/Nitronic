#include "DeviceVK.h"

#include <map>
#include <set>

#include "VkMacros.h"
#include "nvrhi/validation.h"

NAMESPACE {

    std::vector deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
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

        switch (deviceProperties.deviceType) {
            case vk::PhysicalDeviceType::eOther:
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu:
                score += 128;
                break;
            case vk::PhysicalDeviceType::eDiscreteGpu:
                score += 1024;
                break;
            case vk::PhysicalDeviceType::eVirtualGpu:
                score += 64;
                break;
            case vk::PhysicalDeviceType::eCpu:
                score -= 1024;
                break;
        }

        score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

        return score;
    }

    void Device::CreateDeviceVk()
    {
        m_DeviceData = std::make_unique<DeviceDataVk>();
        DEVICE_DATA_OWNED->surface = RENDERER_DATA->surface;

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
            DEVICE_DATA_OWNED->physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("No suitable Vulkan-compatible GPU found.");
        }

#ifdef CONFIG_DEBUG
        for (const auto& [key, value] : candidates | std::views::reverse) {
            ENGINE_TRACE("Device: {}, Score: {}", value.getProperties().deviceName.data(), key);
        }
#endif

        ENGINE_INFO("Using device {}", candidates.rbegin()->second.getProperties().deviceName.data());

        DEVICE_DATA_OWNED->queueFamilyIndices = FindQueueFamilies(DEVICE_DATA_OWNED->physicalDevice, RENDERER_DATA->surface);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = { DEVICE_DATA_OWNED->queueFamilyIndices.graphicsFamily.value(), DEVICE_DATA_OWNED->queueFamilyIndices.presentFamily.value() };
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
        vulkan11Features.shaderDrawParameters = VK_TRUE;

        vk::PhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.timelineSemaphore = VK_TRUE;
        vulkan12Features.pNext = &vulkan11Features;

        vk::PhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.dynamicRendering = VK_TRUE;
        vulkan13Features.synchronization2 = VK_TRUE;
        vulkan13Features.pNext = &vulkan12Features;

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.pNext = &vulkan13Features;

        DEVICE_DATA_OWNED->logicalDevice = DEVICE_DATA_OWNED->physicalDevice.createDevice(deviceCreateInfo);
        DEVICE_DATA_OWNED->graphicsQueue = DEVICE_DATA_OWNED->logicalDevice.getQueue(DEVICE_DATA_OWNED->queueFamilyIndices.graphicsFamily.value(), 0);
        DEVICE_DATA_OWNED->presentQueue = DEVICE_DATA_OWNED->logicalDevice.getQueue(DEVICE_DATA_OWNED->queueFamilyIndices.presentFamily.value(), 0);

        nvrhi::vulkan::DeviceDesc deviceDesc{};
        deviceDesc.errorCB = &NvrhiMessageCallback::GetInstance();
        deviceDesc.physicalDevice = DEVICE_DATA_OWNED->physicalDevice;
        deviceDesc.device = DEVICE_DATA_OWNED->logicalDevice;
        deviceDesc.graphicsQueue = DEVICE_DATA_OWNED->graphicsQueue;
        deviceDesc.graphicsQueueIndex = DEVICE_DATA_OWNED->queueFamilyIndices.graphicsFamily.value();
        deviceDesc.deviceExtensions = deviceExtensions.data();
        deviceDesc.numDeviceExtensions = std::size(deviceExtensions);
        deviceDesc.instance = RENDERER_DATA->instance;
        deviceDesc.instanceExtensions = RENDERER_DATA->instanceExtensions.data();
        deviceDesc.numInstanceExtensions = RENDERER_DATA->instanceExtensions.size();

        nvrhi::DeviceHandle nvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

#ifdef CONFIG_DEBUG
        m_Device = nvrhi::validation::createValidationLayer(nvrhiDevice);
#else
        m_Device = nvrhiDevice;
#endif
    }

    void Device::DestroyDeviceVk()
    {
        m_Device = nullptr;
        DEVICE_DATA_OWNED->logicalDevice.destroy();
    }

}
