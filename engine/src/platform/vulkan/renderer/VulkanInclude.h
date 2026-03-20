//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_VULKANINCLUDE_H
#define NITRONIC_VULKANINCLUDE_H

#include <nvrhi/vulkan.h>

#ifdef NITRONIC_WITH_VULKAN
#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#include <vulkan/vulkan.hpp>
#endif

#endif //NITRONIC_VULKANINCLUDE_H