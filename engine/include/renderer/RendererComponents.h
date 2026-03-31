//
// Created by simon on 29/03/2026.
//

#ifndef NITRONIC_RENDERERCOMPONENTS_H
#define NITRONIC_RENDERERCOMPONENTS_H
#include "Material.h"
#include "Mesh.h"
#include "core/Macros.h"

NAMESPACE {

    struct Rendered
    {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;

        bool cullBackfaces = true;

    private:
        friend class Renderer;

        nvrhi::BufferHandle modelConstantsBuffer;

        nvrhi::GraphicsPipelineHandle graphicsPipeline;

        nvrhi::BindingLayoutHandle bindingLayout;
        nvrhi::BindingSetHandle bindingSet;

        nvrhi::FramebufferInfoEx lastUsedFramebuffer;

        bool initialized = false;

        std::shared_ptr<Material> lastUsedMaterial;
        bool lastUsedCullBackfaces;
    };

}

#endif //NITRONIC_RENDERERCOMPONENTS_H
