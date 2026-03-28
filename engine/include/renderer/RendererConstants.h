//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_RENDERERCONSTANTS_H
#define NITRONIC_RENDERERCONSTANTS_H
#include "core/Macros.h"

#include <nvrhi/nvrhi.h>

NAMESPACE {
    constexpr size_t g_MaxFramesInFlight = 2;
    constexpr auto g_DepthStencilFormat = nvrhi::Format::D32;
}

#endif //NITRONIC_CONSTANTS_H