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
            RENDERER_DATA->m_Instance = vk::createInstance(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(RENDERER_DATA->m_Instance);

            if (!m_Window) {
                throw std::runtime_error("Window is null");
            }

            VkSurfaceKHR rawSurface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(
                RENDERER_DATA->m_Instance,
                m_Window->GetNativeWindow(),
                nullptr,
                &rawSurface
            ));
            RENDERER_DATA->m_Surface = rawSurface;

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

        if (RENDERER_DATA->m_Surface) {
            RENDERER_DATA->m_Instance.destroySurfaceKHR(RENDERER_DATA->m_Surface);
            RENDERER_DATA->m_Surface = VK_NULL_HANDLE;
        }

        if (RENDERER_DATA->m_Instance) {
            RENDERER_DATA->m_Instance.destroy();
            RENDERER_DATA->m_Instance = VK_NULL_HANDLE;
        }
    }

}
