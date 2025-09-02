//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_DEVICE_H
#define NITRONIC_DEVICE_H
#include "core/Macros.h"
#include "core/Enums.h"
#include "nvrhi/nvrhi.h"

NAMESPACE {

    class Device {
    public:
        explicit Device(RenderingBackend backend);
        ~Device();
    private:
        CREATE_BACKEND_FUNCTIONS(CreateDevice)
        CREATE_BACKEND_FUNCTIONS(DestroyDevice)

        RenderingBackend m_Backend;
    protected:
        nvrhi::DeviceHandle m_Device;
    };

}

#endif //NITRONIC_DEVICE_H
