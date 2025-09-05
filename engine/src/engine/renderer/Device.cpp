//
// Created by simon on 02/09/2025.
//

#include "renderer/Device.h"

NAMESPACE {

    Device::Device(const RenderingBackend backend, RendererData* rendererData)
        : m_Backend(backend)
    {
        CREATE_BACKEND_SWITCH(CreateDeviceData, rendererData);

        CREATE_BACKEND_SWITCH(CreateDevice);
    }

    Device::~Device() {
        CREATE_BACKEND_SWITCH(DestroyDevice);
    }
}
