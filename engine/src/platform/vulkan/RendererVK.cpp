//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <nvrhi/vulkan.h>

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE {

    void Renderer::InitVk() {
        std::cout << "Init Vulkan" << std::endl;
    }

    void Renderer::RenderVk(const double deltaTime) {
        std::cout << "Render Vulkan" << std::endl;
    }

    void Renderer::CleanupVk() {
        std::cout << "Cleanup Vulkan" << std::endl;
    }

}
