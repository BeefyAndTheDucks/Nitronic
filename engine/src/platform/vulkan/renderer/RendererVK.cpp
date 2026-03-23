//
// Created by simon on 02/09/2025.
//

#include <iostream>

#include "engine/GlfwInclude.h"

#include "RendererVK.h"

#include "VkMacros.h"
#include "core/Macros.h"
#include "renderer/Constants.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Optional DebugUtils callback (used only if VK_EXT_debug_utils is enabled)
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    switch (messageSeverity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            ENGINE_TRACE("Vulkan: {}", pCallbackData->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            ENGINE_INFO("Vulkan: {}", pCallbackData->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            ENGINE_WARN("Vulkan: {}", pCallbackData->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            ENGINE_ERROR("Vulkan: {}", pCallbackData->pMessage);
            break;
        default:
            break;
    }
    return VK_FALSE;
}

NAMESPACE {

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            //if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm/* && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear*/) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo; // Guaranteed to exist.
    }

    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const Window* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        int width, height;
        window->GetFramebufferSize(&width, &height);

        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    std::vector<SwapChainImage> CreateSwapChain(const Device* device, const Window* window, const vk::SurfaceKHR surface, RendererDataVk* rendererData) {
        auto swapChainSupport = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->GetSwapChainSupport();

        device->GetDevice()->waitForIdle();

        vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

        rendererData->swapChainImageFormat = surfaceFormat.format;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;

        QueueFamilyIndices indices = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->queueFamilyIndices;
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = rendererData->nativeSwapChain;

        rendererData->nativeSwapChain = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.createSwapchainKHR(createInfo);

        if (createInfo.oldSwapchain) {
            DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.destroySwapchainKHR(createInfo.oldSwapchain);
        }

        std::vector<vk::Image> images = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.getSwapchainImagesKHR(rendererData->nativeSwapChain);
        std::vector<SwapChainImage> swapChainImages;

        int imageIndex = 0;
        for (vk::Image image : images) {
            SwapChainImage sci{};

            nvrhi::TextureDesc textureDesc = nvrhi::TextureDesc()
                .setDimension(nvrhi::TextureDimension::Texture2D)
                .setFormat(nvrhi::Format::BGRA8_UNORM)
                .setWidth(extent.width)
                .setHeight(extent.height)
                .setInitialState(nvrhi::ResourceStates::Present)
                .setKeepInitialState(true)
                .setIsRenderTarget(true)
                .setDebugName("SwapChainImage" + std::to_string(imageIndex++));

            sci.nvrhiHandle = device->GetDevice()->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(image), textureDesc);
            swapChainImages.push_back(sci);
        }

        for (auto s : RENDERER_DATA_FROM_BASE(rendererData)->presentSemaphores)
            DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.destroySemaphore(s);
        for (auto s : RENDERER_DATA_FROM_BASE(rendererData)->acquireSemaphores)
            DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.destroySemaphore(s);

        RENDERER_DATA_FROM_BASE(rendererData)->presentSemaphores.clear();
        RENDERER_DATA_FROM_BASE(rendererData)->acquireSemaphores.clear();

        size_t const numPresentSemaphores = swapChainImages.size();
        RENDERER_DATA_FROM_BASE(rendererData)->presentSemaphores.reserve(numPresentSemaphores);
        for (uint32_t i = 0; i < numPresentSemaphores; ++i)
        {
            RENDERER_DATA_FROM_BASE(rendererData)->presentSemaphores.push_back(DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.createSemaphore(vk::SemaphoreCreateInfo()));
        }

        // Create semaphores
        size_t const numAcquireSemaphores = std::max(g_MaxFramesInFlight, swapChainImages.size());
        RENDERER_DATA_FROM_BASE(rendererData)->acquireSemaphores.reserve(numAcquireSemaphores);
        for (uint32_t i = 0; i < numAcquireSemaphores; ++i)
        {
            RENDERER_DATA_FROM_BASE(rendererData)->acquireSemaphores.push_back(DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.createSemaphore(vk::SemaphoreCreateInfo()));
        }

        return swapChainImages;
    }

    void Renderer::InitVk() {
        ENGINE_INFO("Init Vulkan");

        m_RendererData = std::make_unique<RendererDataVk>();

        try {
            vk::detail::DynamicLoader dl;
            VULKAN_HPP_DEFAULT_DISPATCHER.init(dl);
            auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
            if (!vkGetInstanceProcAddr) {
                throw std::runtime_error("Failed to load vkGetInstanceProcAddr");
            }
            VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

            // Application / engine info
            vk::ApplicationInfo appInfo{};
            appInfo.pApplicationName   = "Nitronic";
            appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
            appInfo.pEngineName        = "Nitronic Engine";
            appInfo.engineVersion      = VK_MAKE_API_VERSION(0, 0, 1, 0);
            appInfo.apiVersion         = VK_API_VERSION_1_3;

            std::vector<const char*> instanceExtensions;
            {
                uint32_t glfwExtCount = 0;
                const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
                if (glfwExtensions && glfwExtCount > 0) {
                    instanceExtensions.assign(glfwExtensions, glfwExtensions + glfwExtCount);
                } else {
                    throw std::runtime_error("glfwGetRequiredInstanceExtensions returned no extensions");
                }
            }

            std::vector<const char*> layers;
#ifdef _DEBUG
            layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

            // Check that requested layers exist (when any are requested)
            if (!layers.empty()) {
                auto availableLayers = vk::enumerateInstanceLayerProperties();

                for (const char* requested : layers) {
                    bool found = false;
                    for (const auto& lp : availableLayers) {
                        if (std::strcmp(lp.layerName, requested) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        // If the validation layer isn't available, drop it to avoid instance creation failure.
                        layers.clear();
                        break;
                    }
                }
            }

            // Enumerate all available instance extensions once
            const auto availableExtensions = vk::enumerateInstanceExtensionProperties();
            auto isExtAvailable = [&](const char* name) -> bool {
                for (const auto& ep : availableExtensions) {
                    if (std::strcmp(ep.extensionName, name) == 0)
                        return true;
                }
                return false;
            };

            bool enableDebugUtils = false;
#ifdef _DEBUG
            if (isExtAvailable(vk::EXTDebugUtilsExtensionName)) {
                instanceExtensions.emplace_back(vk::EXTDebugUtilsExtensionName);
                enableDebugUtils = true;
            }
#endif

            // Verify required extensions (GLFW + any we explicitly added above)
            for (const char* requested : instanceExtensions) {
                if (!isExtAvailable(requested)) {
                    // These are considered required because they've been added explicitly (e.g., by GLFW).
                    throw std::runtime_error(std::string("Required instance extension not available: ") + requested);
                }
            }

            vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
            if (enableDebugUtils) {
                debugUtilsCreateInfo.messageSeverity =
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
                debugUtilsCreateInfo.messageType =
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
                debugUtilsCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;
                debugUtilsCreateInfo.pUserData = nullptr;
            }

            vk::InstanceCreateInfo createInfo{};
            createInfo.pApplicationInfo        = &appInfo;
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(instanceExtensions.size());
            createInfo.ppEnabledExtensionNames = instanceExtensions.data();
            createInfo.enabledLayerCount       = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames     = layers.empty() ? nullptr : layers.data();
            createInfo.pNext                   = enableDebugUtils ? &debugUtilsCreateInfo : nullptr;

            // Create the instance and initialize the dispatcher with instance-level functions
            RENDERER_DATA_OWNED->instance = vk::createInstance(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(RENDERER_DATA_OWNED->instance);

            if (!m_Window) {
                throw std::runtime_error("Window is null");
            }

            VkSurfaceKHR rawSurface = VK_NULL_HANDLE;

            VK_CHECK(glfwCreateWindowSurface(
                RENDERER_DATA_OWNED->instance,
                m_Window->GetNativeWindow(),
                nullptr,
                &rawSurface
            ));
            RENDERER_DATA_OWNED->surface = rawSurface;

            ENGINE_INFO("Vulkan initialized.");
        }
        catch (const vk::SystemError& e) {
            ENGINE_CRITICAL("Vulkan error during instance creation: {}", e.what());
            throw;
        }
        catch (const std::exception& e) {
            ENGINE_CRITICAL("Error during Vulkan init: {}", e.what());
            throw;
        }
    }

    void Renderer::InitAfterDeviceCreationVk() {
        m_SwapChainImages = CreateSwapChain(m_Device.get(), m_Window, RENDERER_DATA_OWNED->surface, RENDERER_DATA_OWNED);
        m_SwapChainIndex = 0;
    }

    void Renderer::BeginFrameVk() {
        vk::Result res{};

        constexpr int maxAttempts = 3;
        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            const auto& semaphore = RENDERER_DATA_OWNED->acquireSemaphores[RENDERER_DATA_OWNED->acquireSemaphoreIndex];
            res = DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->logicalDevice.acquireNextImageKHR(
                RENDERER_DATA_OWNED->nativeSwapChain,
                std::numeric_limits<uint64_t>::max() - 1, // timeout
                semaphore,
                vk::Fence(),
                &m_SwapChainIndex);

            if (res == vk::Result::eErrorOutOfDateKHR && attempt < maxAttempts)
            {
                m_SwapChainImages = CreateSwapChain(m_Device.get(), m_Window, RENDERER_DATA_OWNED->surface, RENDERER_DATA_OWNED);
                m_SwapChainIndex = 0;
                GenerateBackbuffers();
            }
            else
                break;
        }

        const auto& semaphore = RENDERER_DATA_OWNED->acquireSemaphores[RENDERER_DATA_OWNED->acquireSemaphoreIndex];
        RENDERER_DATA_OWNED->acquireSemaphoreIndex = (RENDERER_DATA_OWNED->acquireSemaphoreIndex + 1) % RENDERER_DATA_OWNED->acquireSemaphores.size();

        if (res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR) // Suboptimal is considered a success
        {
            const nvrhi::vulkan::DeviceHandle vulkanDevice = static_cast<nvrhi::vulkan::IDevice*>(
                m_Device->GetDevice()->getNativeObject(nvrhi::ObjectTypes::Nvrhi_VK_Device));
            // Schedule the wait. The actual wait operation will be submitted when the app executes any command list.
            vulkanDevice->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
        }
    }

    void Renderer::PresentFrameVk() {
        const nvrhi::vulkan::DeviceHandle vulkanNvrhiDevice = static_cast<nvrhi::vulkan::IDevice*>(
                m_Device->GetDevice()->getNativeObject(nvrhi::ObjectTypes::Nvrhi_VK_Device));

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.swapchainCount = 1;

        constexpr int maxAttempts = 3;
        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            const auto& semaphore = RENDERER_DATA_OWNED->presentSemaphores[m_SwapChainIndex];
            vulkanNvrhiDevice->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);

            // NVRHI buffers the semaphores and signals them when something is submitted to a queue.
            // Call 'executeCommandLists' with no command lists to actually signal the semaphore.
            vulkanNvrhiDevice->executeCommandLists(nullptr, 0);
            presentInfo.pWaitSemaphores = &semaphore;
            presentInfo.pSwapchains = &RENDERER_DATA_OWNED->nativeSwapChain;
            presentInfo.pImageIndices = &m_SwapChainIndex;

            try {
                const vk::Result res = DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->presentQueue.presentKHR(&presentInfo);

                if (res == vk::Result::eErrorOutOfDateKHR) {
                    m_SwapChainImages = CreateSwapChain(m_Device.get(), m_Window, RENDERER_DATA_OWNED->surface, RENDERER_DATA_OWNED);
                    m_SwapChainIndex = 0;
                    GenerateBackbuffers();
                    return;
                }
                if (res != vk::Result::eSuccess && res != vk::Result::eSuboptimalKHR) {
                    ENGINE_CRITICAL("Failed to present swap chain image: {}", vk::to_string(res));
                    std::abort();
                }

                break;
            } catch (const vk::SystemError& e) {
                ENGINE_CRITICAL("Vulkan error during present: {}", e.what());
                std::abort();
            }
        }

        while (m_FramesInFlight.size() >= g_MaxFramesInFlight) {
            auto query = m_FramesInFlight.front();
            m_FramesInFlight.pop();

            vulkanNvrhiDevice->waitEventQuery(query);

            m_QueryPool.push_back(query);
        }

        nvrhi::EventQueryHandle query;
        if (!m_QueryPool.empty()) {
            query = m_QueryPool.back();
            m_QueryPool.pop_back();
        } else {
            query = vulkanNvrhiDevice->createEventQuery();
        }

        vulkanNvrhiDevice->resetEventQuery(query);
        vulkanNvrhiDevice->setEventQuery(query, nvrhi::CommandQueue::Graphics);
        m_FramesInFlight.push(query);
    }

    void Renderer::CleanupPreDeviceVk() {
        if (RENDERER_DATA_OWNED->nativeSwapChain) {
            DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->logicalDevice.destroySwapchainKHR(RENDERER_DATA_OWNED->nativeSwapChain);
            RENDERER_DATA_OWNED->nativeSwapChain = VK_NULL_HANDLE;
        }

        for (vk::Semaphore semaphore : RENDERER_DATA_OWNED->acquireSemaphores) {
            DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->logicalDevice.destroySemaphore(semaphore);
        }

        for (vk::Semaphore semaphore : RENDERER_DATA_OWNED->presentSemaphores) {
            DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->logicalDevice.destroySemaphore(semaphore);
        }
    }


    void Renderer::CleanupVk() {
        if (RENDERER_DATA_OWNED->surface) {
            RENDERER_DATA_OWNED->instance.destroySurfaceKHR(RENDERER_DATA_OWNED->surface);
            RENDERER_DATA_OWNED->surface = VK_NULL_HANDLE;
        }

        if (RENDERER_DATA_OWNED->instance) {
            RENDERER_DATA_OWNED->instance.destroy();
            RENDERER_DATA_OWNED->instance = VK_NULL_HANDLE;
        }
    }

}
