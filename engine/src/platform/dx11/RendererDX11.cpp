//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <nvrhi/d3d11.h>

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE {

    void Renderer::InitDX11() {
        std::cout << "Init DX11" << std::endl;
    }

    void Renderer::InitAfterDeviceCreationDX11() {
        std::cout << "Init after device creation DX11" << std::endl;
    }

    void Renderer::RenderDX11(const double deltaTime) {
        std::cout << "Render DX11" << std::endl;
    }

    void Renderer::CleanupPreDeviceDX11() {
        std::cout << "Cleanup pre device DX11" << std::endl;
    }

    void Renderer::CleanupDX11() {
        std::cout << "Cleanup DX11" << std::endl;
    }

}
