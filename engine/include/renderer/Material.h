//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MATERIAL_H
#define NITRONIC_MATERIAL_H
#include "core/Macros.h"
#include "nvrhi/nvrhi.h"

NAMESPACE {

    class Material {
    public:
        Material(const nvrhi::IDevice& device, const std::string& fragmentSourcePath, const std::string& vertexSourcePath);
        ~Material();

    private:
        nvrhi::ShaderHandle m_FragmentShader;
        nvrhi::ShaderHandle m_VertexShader;
    };

}

#endif //NITRONIC_MATERIAL_H