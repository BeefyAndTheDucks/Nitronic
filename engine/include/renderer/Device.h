﻿//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_DEVICE_H
#define NITRONIC_DEVICE_H
#include "RendererTypes.h"
#include "core/Macros.h"
#include "core/Enums.h"
#include "nvrhi/nvrhi.h"

NAMESPACE {

    class Device {
    public:
        Device(RenderingBackend backend, RendererData* rendererData);
        ~Device();

        [[nodiscard]] nvrhi::DeviceHandle GetDevice() const { return m_Device; }
        [[nodiscard]] DeviceData* GetDeviceData() const { return m_DeviceData; }
    private:
        CREATE_BACKEND_FUNCTIONS(CreateDevice)
        CREATE_BACKEND_FUNCTIONS(DestroyDevice)

        RenderingBackend m_Backend;
        RendererData* m_RendererData;
        DeviceData* m_DeviceData;
        nvrhi::DeviceHandle m_Device;
    };

}

#endif //NITRONIC_DEVICE_H
