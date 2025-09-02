//
// Created by simon on 02/09/2025.
//

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "RendererVK.h"
#include "core/Macros.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

NAMESPACE {

    vk::Instance g_VkInstance;
    vk::SurfaceKHR g_VkSurface;

    void Renderer::InitVk() {
        std::cout << "Init Vulkan" << std::endl;

        try {
            vk::detail::DynamicLoader dl;
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
            g_VkInstance = vk::createInstance(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(g_VkInstance);

            if (!m_Window) {
                throw std::runtime_error("Window is null");
            }

            VkSurfaceKHR rawSurface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(
                g_VkInstance,
                m_Window->GetNativeWindow(),
                nullptr,
                &rawSurface
            ));
            g_VkSurface = rawSurface;

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

    void Renderer::RenderVk(const double deltaTime) {
        std::cout << "Render Vulkan" << std::endl;
    }

    void Renderer::CleanupVk() {
        std::cout << "Cleanup Vulkan" << std::endl;

        if (g_VkSurface) {
            g_VkInstance.destroySurfaceKHR(g_VkSurface);
            g_VkSurface = VK_NULL_HANDLE;
        }

        if (g_VkInstance) {
            g_VkInstance.destroy();
            g_VkInstance = VK_NULL_HANDLE;
        }

        delete m_Device;
    }

}
