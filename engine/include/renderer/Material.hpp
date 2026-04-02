//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MATERIAL_HPP
#define NITRONIC_MATERIAL_HPP
#include "ShaderCache.hpp"
#include "core/Macros.hpp"

NAMESPACE {

    struct Material {
        ShaderKey fragmentShader;
        ShaderKey vertexShader;

        std::vector<nvrhi::VertexAttributeDesc> vertexAttributes;
    };

}

#endif //NITRONIC_MATERIAL_HPP