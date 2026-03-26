//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERERVK_H
#define NITRONIC_RENDERERVK_H

#include "renderer/Renderer.h"

#include "VulkanInclude.h"
#include "VkHelpers.h"

#ifndef VK_CHECK
#define VK_CHECK(x) \
    do { \
        VkResult err__ = (x); \
        if (err__ != VK_SUCCESS) { \
            ENGINE_ABORT("Vulkan error: {} at {}:{}", VkResultToString(err__), __FILE__, __LINE__); \
        } \
    } while (0)
#endif

NAMESPACE {

    struct RendererDataVk : RendererData {
        vk::Instance instance;
        vk::SurfaceKHR surface;

        std::vector<const char*> instanceExtensions;

        vk::SwapchainKHR nativeSwapChain;
        vk::Format swapChainImageFormat;

        std::vector<vk::Semaphore> acquireSemaphores;
        std::vector<vk::Semaphore> presentSemaphores;
        uint32_t acquireSemaphoreIndex = 0;
    };

}

#endif //NITRONIC_RENDERERVK_H