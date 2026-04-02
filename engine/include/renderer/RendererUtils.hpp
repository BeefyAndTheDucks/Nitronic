//
// Created by beefy on 3/31/26.
//

#ifndef NITRONIC_RENDERERUTILS_HPP
#define NITRONIC_RENDERERUTILS_HPP
#include "core/Macros.hpp"
#include "nvrhi/nvrhi.h"

NAMESPACE {
    inline bool AreFramebuffersCompatible(nvrhi::FramebufferInfoEx a, nvrhi::FramebufferInfoEx b) {
        for (size_t i = 0; i < a.colorFormats.size(); ++i)
        {
            if (a.colorFormats[i] != b.colorFormats[i])
                return false;
        }

        return a.depthFormat == b.depthFormat
            && a.sampleCount == b.sampleCount;
    }
}

#endif //NITRONIC_RENDERERUTILS_HPP
