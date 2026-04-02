//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MESH_HPP
#define NITRONIC_MESH_HPP
#include <vector>

#include <nvrhi/nvrhi.h>

#include "RendererTypes.hpp"
#include "core/Macros.hpp"

NAMESPACE {

    class Mesh {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
        ~Mesh() = default;

        void Initialize(nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

        [[nodiscard]] bool IsInitialized() const { return m_Initialized; }

        [[nodiscard]] nvrhi::BufferHandle GetVertexBuffer() const { return m_VertexBuffer; }
        [[nodiscard]] nvrhi::BufferHandle GetIndexBuffer() const { return m_IndexBuffer; }

        [[nodiscard]] nvrhi::VertexBufferBinding GetVertexBufferBinding() const { return nvrhi::VertexBufferBinding(m_VertexBuffer, 0, 0); }
        [[nodiscard]] nvrhi::IndexBufferBinding GetIndexBufferBinding() const { return nvrhi::IndexBufferBinding(m_IndexBuffer, k_IndexBufferFormat, 0); }

        [[nodiscard]] uint32_t GetNumVertices() const { return static_cast<uint32_t>(std::size(m_Indices)); }

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        bool m_Initialized = false;

        static constexpr auto k_IndexBufferFormat = nvrhi::Format::R32_UINT;
    };

}


#endif //NITRONIC_MESH_HPP