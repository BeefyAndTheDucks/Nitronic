//
// Created by simon on 02/09/2025.
//

#include "core/Macros.h"

#include "renderer/Renderer.h"

NAMESPACE
{

    void Renderer::InitDX12() {
        ENGINE_INFO("Init DX12");
    }

    void Renderer::InitAfterDeviceCreationDX12() {
        ENGINE_INFO("Init after device creation DX12");
    }

    void Renderer::BeginFrameDX12() {
        ENGINE_INFO("BeginFrame DX12");
    }

    void Renderer::PresentFrameDX12() {
        ENGINE_INFO("PresentFrame DX12");
    }

    void Renderer::CleanupPreDeviceDX12() {
        ENGINE_INFO("Cleanup pre device DX12");
    }

    void Renderer::CleanupDX12() {
        ENGINE_INFO("Cleanup DX12");
    }

}
