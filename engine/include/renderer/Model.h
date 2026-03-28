//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MODEL_H
#define NITRONIC_MODEL_H
#include <nvrhi/nvrhi.h>
#include "Material.h"
#include "Mesh.h"
#include "PSOCache.h"
#include "Transform.h"

NAMESPACE {

    struct alignas(16) ModelConstants {
        glm::mat4 viewProj;
        glm::mat4 model;
        glm::mat4 normalMatrix;
    };

    class Model {
    public:
        Model(const Mesh& mesh, const Material& material, const Transform& transform, bool isStatic);
        ~Model() = default;

        [[nodiscard]] bool IsInitialized() const { return m_Initialized; }

        void Initialize(nvrhi::IDevice *device, const nvrhi::CommandListHandle &commandList, PSOCache& psoCache, const nvrhi::FramebufferHandle &fb);

        /// Check if the cached PSO is compatible with the given framebuffer
        [[nodiscard]] bool IsCompatibleWith(const nvrhi::FramebufferHandle& fb) const;

        /// Rebuild the PSO for a different framebuffer format
        void RebuildPipeline(PSOCache& psoCache, const nvrhi::FramebufferHandle &fb);

        void Render(const nvrhi::CommandListHandle &commandList, const nvrhi::FramebufferHandle& fb, const glm::mat4& viewProjectionMatrix) const;

        [[nodiscard]] Transform* GetMutableTransform() { return &m_Transform; }

        [[nodiscard]] const Transform& GetTransform() const { return m_Transform; }
        void SetTransform(const Transform& transform) { m_Transform = transform; }

    private:
        Transform m_Transform;
        Mesh m_Mesh;
        Material m_Material;

        nvrhi::BufferHandle m_ModelConstantsBuffer;

        nvrhi::GraphicsPipelineHandle m_GraphicsPipeline;

        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSet;

        nvrhi::FramebufferInfoEx m_CachedFramebufferInfo;

        bool m_IsStatic;

        bool m_Initialized = false;
    };

}


#endif //NITRONIC_MODEL_H