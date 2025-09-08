//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERERVK_H
#define NITRONIC_RENDERERVK_H

#include "renderer/Renderer.h"

#include "VulkanInclude.h"

#ifndef VK_CHECK
#define VK_CHECK(x) \
    do { \
        VkResult err__ = (x); \
        if (err__ != VK_SUCCESS) { \
            std::cerr << "Vulkan error: " << err__ << " at " << __FILE__ \
                      << ":" << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while (0)
#endif

NAMESPACE {

    struct RendererDataVk : RendererData {
        vk::Instance instance;
        vk::SurfaceKHR surface;

        vk::SwapchainKHR nativeSwapChain;

        std::vector<vk::Semaphore> acquireSemaphores;
        std::vector<vk::Semaphore> presentSemaphores;
        uint32_t acquireSemaphoreIndex = 0;
    };

}

#endif //NITRONIC_RENDERERVK_H