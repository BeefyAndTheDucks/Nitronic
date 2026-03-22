//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_SHADERS_H
#define NITRONIC_SHADERS_H
#include "Renderer.h"
#include "nvrhi/nvrhi.h"

NAMESPACE {

    constexpr auto g_ShaderBasicName = "Basic";
    const ShaderKey g_ShaderBasicVertex = {g_ShaderBasicName, nvrhi::ShaderType::Vertex};
    const ShaderKey g_ShaderBasicFragment = {g_ShaderBasicName, nvrhi::ShaderType::Pixel};
    const std::vector g_ShaderBasicAttributes = {
        nvrhi::VertexAttributeDesc()
            .setName("a_Position")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("a_Normal")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, normal))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("a_TexCoord")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, texCoord))
            .setElementStride(sizeof(Vertex)),
    };

    constexpr auto g_ShaderEmptyName = "Empty";
    const ShaderKey g_ShaderEmptyVertex = {g_ShaderEmptyName, nvrhi::ShaderType::Vertex};
    const ShaderKey g_ShaderEmptyFragment = {g_ShaderEmptyName, nvrhi::ShaderType::Pixel};
    const std::vector g_ShaderEmptyAttributes = {
        nvrhi::VertexAttributeDesc()
            .setName("a_Position")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("a_Normal")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, normal))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("a_TexCoord")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, texCoord))
            .setElementStride(sizeof(Vertex)),
    };

}

#endif //NITRONIC_SHADERS_H