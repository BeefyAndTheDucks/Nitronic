//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERERVK_H
#define NITRONIC_RENDERERVK_H

#include "renderer/Renderer.h"
#include <nvrhi/vulkan.h>

#ifdef NITRONIC_WITH_VULKAN
#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#include <vulkan/vulkan.hpp>
#endif

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

    extern vk::Instance g_VkInstance;
    extern vk::SurfaceKHR g_VkSurface;

}

#endif //NITRONIC_RENDERERVK_H