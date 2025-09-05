//
// Created by simon on 03/09/2025.
//

#ifndef NITRONIC_RENDERERTYPES_H
#define NITRONIC_RENDERERTYPES_H
#include "core/Macros.h"

NAMESPACE {

    struct RendererData {
        virtual ~RendererData() = default;
    };

    struct DeviceData {
        RendererData* m_RendererData;

        virtual ~DeviceData() = default;
    };

}

#endif //NITRONIC_RENDERERTYPES_H