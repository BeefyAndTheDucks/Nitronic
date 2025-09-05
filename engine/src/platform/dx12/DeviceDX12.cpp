//
// Created by simon on 02/09/2025.
//

#include "core/Macros.h"
#include "renderer/Device.h"
NAMESPACE {

    void Device::CreateDeviceDataDX12(RendererData *rendererData) {
        m_DeviceData = new DeviceData();
        m_DeviceData->m_RendererData = rendererData;
    }

    void Device::CreateDeviceDX12() {

    }

    void Device::DestroyDeviceDX12() {

    }

}