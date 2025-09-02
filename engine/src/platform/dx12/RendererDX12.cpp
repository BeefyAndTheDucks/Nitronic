//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <nvrhi/d3d12.h>

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE {

    void Renderer::InitDX12() {
        std::cout << "Init DX12" << std::endl;
    }

    void Renderer::RenderDX12(const double deltaTime) {
        std::cout << "Render DX12" << std::endl;
    }

    void Renderer::CleanupDX12() {
        std::cout << "Cleanup DX12" << std::endl;
    }

}
