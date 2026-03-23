//
// Created by simon on 21/03/2026.
//

#include "engine/Log.h"

#include "renderer/Mesh.h"

NAMESPACE
{

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : m_Vertices(vertices), m_Indices(indices) {}

    void Mesh::Initialize(nvrhi::IDevice* device, nvrhi::ICommandList* commandList) {
        if (m_Initialized) {
            ENGINE_WARN("Mesh already initialized.");
            return;
        }

        const auto vertexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(m_Vertices.size() * sizeof(Vertex))
            .setIsVertexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Vertex Buffer");
        m_VertexBuffer = device->createBuffer(vertexBufferDesc);

        const auto indexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(m_Indices.size() * sizeof(uint32_t))
            .setIsIndexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::IndexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Index Buffer");
        m_IndexBuffer = device->createBuffer(indexBufferDesc);

        commandList->writeBuffer(m_VertexBuffer, m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
        commandList->writeBuffer(m_IndexBuffer, m_Indices.data(), m_Indices.size() * sizeof(uint32_t));

        m_Initialized = true;
    }
}
