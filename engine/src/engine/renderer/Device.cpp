//
// Created by simon on 02/09/2025.
//

#include "renderer/Device.hpp"

NAMESPACE {

    Device::Device(const RenderingBackend backend, RendererData* rendererData)
        : m_RendererData(rendererData), m_Backend(backend)
    {
        CREATE_BACKEND_SWITCH(CreateDevice);
    }

    Device::~Device() {
        CREATE_BACKEND_SWITCH(DestroyDevice);
    }
}
