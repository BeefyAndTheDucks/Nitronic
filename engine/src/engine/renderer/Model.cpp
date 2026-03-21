//
// Created by simon on 21/03/2026.
//

#include "renderer/Model.h"

#include "nvrhi/utils.h"

NAMESPACE {

    Model::Model(const Mesh& mesh, const Material& material, const Transform& transform, const bool isStatic)
        : m_Transform(transform), m_Mesh(mesh), m_Material(material), m_IsStatic(isStatic) {}

    void Model::Initialize(nvrhi::IDevice *device, const nvrhi::CommandListHandle &commandList, PSOCache& psoCache,
                           const nvrhi::BindingSetDesc &frameConstantsBindingSet, const nvrhi::FramebufferHandle &fb) {
        if (m_Initialized) {
            std::cout << "Model already initialized." << std::endl;
            return;
        }

        m_Mesh.Initialize(device, commandList);

        constexpr nvrhi::RenderState renderState = nvrhi::RenderState()
                .setDepthStencilState(nvrhi::DepthStencilState()
                    .setDepthTestEnable(false)
                    .setStencilEnable(false))
                .setRasterState(nvrhi::RasterState()
                    .setFrontCounterClockwise(true));

        if (!nvrhi::utils::CreateBindingSetAndLayout(device, nvrhi::ShaderType::All, 0, frameConstantsBindingSet, m_BindingLayout, m_BindingSet)) {
            throw std::runtime_error("Failed to create binding set and layout.");
        }

        PSOKey key{};
        key.vertexShader = m_Material.vertexShader;
        key.fragmentShader = m_Material.fragmentShader;
        key.renderState = renderState;
        key.primType = nvrhi::PrimitiveType::TriangleList;
        key.vertexAttributes = m_Material.vertexAttributes;
        key.bindingLayout = m_BindingLayout;
        key.framebufferInfo = fb->getFramebufferInfo();

        m_GraphicsPipeline = psoCache.get(key);

        m_Initialized = true;
    }

    void Model::Render(const nvrhi::CommandListHandle &commandList, const nvrhi::FramebufferHandle& fb) const {
        const auto graphicsState = nvrhi::GraphicsState()
            .setPipeline(m_GraphicsPipeline)
            .setFramebuffer(fb)
            .addVertexBuffer(nvrhi::VertexBufferBinding(m_Mesh.GetVertexBuffer()))
            .setIndexBuffer(nvrhi::IndexBufferBinding(m_Mesh.GetIndexBuffer()))
            .addBindingSet(m_BindingSet)
            .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(fb->getFramebufferInfo().width, fb->getFramebufferInfo().height)));

        commandList->setGraphicsState(graphicsState);

        nvrhi::DrawArguments drawArgs{};
        drawArgs.vertexCount = m_Mesh.GetNumVertices();
        commandList->drawIndexed(drawArgs);
    }

}
