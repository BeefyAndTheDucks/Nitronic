//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MESH_H
#define NITRONIC_MESH_H
#include <vector>

#include <nvrhi/nvrhi.h>

#include "RendererTypes.h"
#include "core/Macros.h"

NAMESPACE {

    class Mesh {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
        ~Mesh() = default;

        void Initialize(nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

        [[nodiscard]] bool IsInitialized() const { return m_Initialized; }

        [[nodiscard]] nvrhi::BufferHandle GetVertexBuffer() const { return m_VertexBuffer; }
        [[nodiscard]] nvrhi::BufferHandle GetIndexBuffer() const { return m_IndexBuffer; }
        [[nodiscard]] uint32_t GetNumVertices() const { return static_cast<uint32_t>(std::size(m_Indices)); }

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        bool m_Initialized = false;
    };

}


#endif //NITRONIC_MESH_H