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
        glm::mat4 mvp;
    };

    class Model {
    public:
        Model(const Mesh &mesh, const Material &material, const Transform &transform, bool isStatic);
        ~Model() = default;

        [[nodiscard]] bool IsInitialized() const { return m_Initialized; }

        void Initialize(nvrhi::IDevice *device, const nvrhi::CommandListHandle &commandList, PSOCache& psoCache, const nvrhi::FramebufferHandle &fb);

        void Render(const nvrhi::CommandListHandle &commandList, const nvrhi::FramebufferHandle& fb, const glm::mat4& viewProjectionMatrix) const;
    private:
        Transform m_Transform;
        Mesh m_Mesh;
        Material m_Material;

        nvrhi::BufferHandle m_ModelConstantsBuffer;

        nvrhi::GraphicsPipelineHandle m_GraphicsPipeline;

        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSet;

        bool m_IsStatic;

        bool m_Initialized = false;
    };

}


#endif //NITRONIC_MODEL_H