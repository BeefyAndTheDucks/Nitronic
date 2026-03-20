//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <nvrhi/d3d12.h>

#include "renderer/Renderer.h"
#include "core/Macros.h"
#include "util/IOUtils.h"

NAMESPACE {

    void Renderer::InitDX12() {
        std::cout << "Init DX12" << std::endl;
    }

    void Renderer::InitAfterDeviceCreationDX12() {
        std::cout << "Init after device creation DX12" << std::endl;
    }

    void Renderer::BeginFrameDX12() {
        std::cout << "BeginFrame DX12" << std::endl;
    }

    void Renderer::PresentFrameDX12() {
        std::cout << "PresentFrame DX12" << std::endl;
    }

    void Renderer::CleanupPreDeviceDX12() {
        std::cout << "Cleanup pre device DX12" << std::endl;
    }

    void Renderer::CleanupDX12() {
        std::cout << "Cleanup DX12" << std::endl;
    }

}
