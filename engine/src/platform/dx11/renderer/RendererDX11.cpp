//
// Created by simon on 02/09/2025.
//

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE
{

    void Renderer::InitDX11() {
        ENGINE_INFO("Init DX11");
    }

    void Renderer::InitAfterDeviceCreationDX11() {
        ENGINE_INFO("Init after device creation DX11");
    }

    void Renderer::BeginFrameDX11() {
        ENGINE_INFO("BeginFrame DX11");
    }

    void Renderer::PresentFrameDX11() {
        ENGINE_INFO("PresentFrame DX11");
    }

    void Renderer::CleanupPreDeviceDX11() {
        ENGINE_INFO("Cleanup pre device DX11");
    }

    void Renderer::CleanupDX11() {
        ENGINE_INFO("Cleanup DX11");
    }

}
