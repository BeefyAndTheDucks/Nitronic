//
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
    private:
        CREATE_BACKEND_FUNCTIONS(CreateDeviceData, RendererData* rendererData)
        CREATE_BACKEND_FUNCTIONS(CreateDevice)
        CREATE_BACKEND_FUNCTIONS(DestroyDevice)

        RenderingBackend m_Backend;
        DeviceData* m_DeviceData;
        nvrhi::DeviceHandle m_Device;
    };

}

#endif //NITRONIC_DEVICE_H
