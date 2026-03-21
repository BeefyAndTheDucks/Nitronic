//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MATERIAL_H
#define NITRONIC_MATERIAL_H
#include "ShaderCache.h"
#include "core/Macros.h"

NAMESPACE {

    struct Material {
        ShaderKey fragmentShader;
        ShaderKey vertexShader;

        std::vector<nvrhi::VertexAttributeDesc> vertexAttributes;
    };

}

#endif //NITRONIC_MATERIAL_H