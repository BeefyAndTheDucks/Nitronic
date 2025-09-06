//
// Created by simon on 02/09/2025.
//

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "RendererVK.h"

#include "VkMacros.h"
#include "core/Macros.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

NAMESPACE {

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
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
        glfwGetFramebufferSize(window->GetNativeWindow(), &width, &height);

        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    std::vector<SwapChainImage> CreateSwapChain(const Device* device, const Window* window, const vk::SurfaceKHR surface, RendererDataVk* rendererData) {
        SwapChainSupportDetails swapChainSupport = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->swapChainSupport;

        vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

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
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

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
        createInfo.oldSwapchain = VK_NULL_HANDLE; // TODO: Recreate swapchain on resize

        rendererData->nativeSwapChain = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.createSwapchainKHR(createInfo);

        std::vector<vk::Image> images = DEVICE_DATA_FROM_BASE(device->GetDeviceData())->logicalDevice.getSwapchainImagesKHR(rendererData->nativeSwapChain);
        std::vector<SwapChainImage> swapChainImages;
        for (vk::Image image : images) {
            SwapChainImage sci{};
            //sci.image = image;

            nvrhi::TextureDesc textureDesc{};
            textureDesc.width = extent.width;
            textureDesc.height = extent.height;
            textureDesc.format = nvrhi::Format::SBGRA8_UNORM;
            textureDesc.debugName = "Swap chain image";
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;
            textureDesc.isRenderTarget = true;

            sci.nvrhiHandle = device->GetDevice()->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(image), textureDesc);
            swapChainImages.push_back(sci);
        }

        return swapChainImages;
    }

    void Renderer::InitVk() {
        std::cout << "Init Vulkan" << std::endl;

        m_RendererData = new RendererDataVk();

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

            std::vector<const char*> extensions;
            {
                uint32_t glfwExtCount = 0;
                const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
                if (glfwExts && glfwExtCount > 0) {
                    extensions.assign(glfwExts, glfwExts + glfwExtCount);
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

            // Check that requested extensions exist
            {
                auto availableExts = vk::enumerateInstanceExtensionProperties();
                for (const char* requested : extensions) {
                    bool found = false;
                    for (const auto& ep : availableExts) {
                        if (std::strcmp(ep.extensionName, requested) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::runtime_error(std::string("Required instance extension not available: ") + requested);
                    }
                }
            }

            vk::InstanceCreateInfo createInfo{};
            createInfo.pApplicationInfo        = &appInfo;
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledLayerCount       = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames     = layers.empty() ? nullptr : layers.data();

            // Create the instance and initialize the dispatcher with instance-level functions
            RENDERER_DATA->instance = vk::createInstance(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(RENDERER_DATA->instance);

            if (!m_Window) {
                throw std::runtime_error("Window is null");
            }

            VkSurfaceKHR rawSurface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(
                RENDERER_DATA->instance,
                m_Window->GetNativeWindow(),
                nullptr,
                &rawSurface
            ));
            RENDERER_DATA->surface = rawSurface;

            std::cout << "Vulkan instance and surface created." << std::endl;
        }
        catch (const vk::SystemError& e) {
            std::cerr << "Vulkan error during instance creation: " << e.what() << std::endl;
            throw;
        }
        catch (const std::exception& e) {
            std::cerr << "Error during Vulkan init: " << e.what() << std::endl;
            throw;
        }
    }

    void Renderer::InitAfterDeviceCreationVk() {
        m_SwapChainImages = CreateSwapChain(m_Device, m_Window, RENDERER_DATA->surface, RENDERER_DATA);
    }

    void Renderer::RenderVk(const double deltaTime) {
        //std::cout << "Render Vulkan" << std::endl;
    }

    void Renderer::CleanupPreDeviceVk() {
        if (RENDERER_DATA->nativeSwapChain) {
            DEVICE_DATA_FROM_BASE(m_Device->GetDeviceData())->logicalDevice.destroySwapchainKHR(RENDERER_DATA->nativeSwapChain);
            RENDERER_DATA->nativeSwapChain = VK_NULL_HANDLE;
        }
    }


    void Renderer::CleanupVk() {
        std::cout << "Cleanup Vulkan" << std::endl;

        if (RENDERER_DATA->surface) {
            RENDERER_DATA->instance.destroySurfaceKHR(RENDERER_DATA->surface);
            RENDERER_DATA->surface = VK_NULL_HANDLE;
        }

        if (RENDERER_DATA->instance) {
            RENDERER_DATA->instance.destroy();
            RENDERER_DATA->instance = VK_NULL_HANDLE;
        }
    }

}
