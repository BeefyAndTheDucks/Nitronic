//
// Created by simon on 02/09/2025.
//

#include "renderer/Renderer.hpp"

#include <filesystem>
#include <tracy/Tracy.hpp>
#include <imgui.h>
#include <nvrhi/utils.h>

#include "renderer/Camera.hpp"
#include "renderer/ImGuiRenderer.hpp"
#include "renderer/Shaders.hpp"

#include "core/Macros.hpp"
#include "core/Constants.hpp"
#include "engine/BasicComponents.hpp"
#include "engine/Event.hpp"
#include "renderer/RendererComponents.hpp"
#include "renderer/RendererConstants.hpp"
#include "renderer/RendererUtils.hpp"

NAMESPACE {

    Renderer::Renderer(const RenderingBackend backend, Window* window, EventBus& eventBus)
        : m_Backend(backend), m_Window(window), m_EventBus(eventBus), m_RendererData(nullptr), m_ViewProjectionMatrix(glm::identity<glm::mat4>())
    {
        ZoneScoped

        ENGINE_INFO("Using {} backend.", RenderingBackendToString(backend));

        CREATE_BACKEND_SWITCH(Init);
        m_Device = std::make_unique<Device>(m_Backend, m_RendererData.get());
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);

        m_ShaderCache = std::make_unique<ShaderCache>(
            m_Device->GetDevice(),
            g_ShadersDirectory,
            backend
        );

        m_PSOCache = std::make_unique<PSOCache>(m_Device->GetDevice(), *m_ShaderCache);

        if (nvrhi::BindingSetHandle unusedBindingSet; !nvrhi::utils::CreateBindingSetAndLayout(m_Device->GetDevice(), nvrhi::ShaderType::All, 0, nvrhi::BindingSetDesc(), m_BindingLayout, m_BindingSet)) {
            throw std::runtime_error("Failed to create binding set and layout.");
        }

        GenerateBackbuffers();

        m_ImGuiRenderer = std::make_unique<ImGuiRenderer>(backend, window, m_RendererData.get(), m_Device->GetDeviceData(), m_EventBus);

        const nvrhi::SamplerDesc samplerDesc = nvrhi::SamplerDesc()
            .setAllAddressModes(nvrhi::SamplerAddressMode::Repeat)
            .setMaxAnisotropy(1.0f);
        m_Sampler = m_Device->GetDevice()->createSampler(samplerDesc);

        m_EventBus.subscribe<FramebufferResizeEvent, &Renderer::OnFramebufferResized>(*this);
    }

    Renderer::~Renderer() {
        m_Device->GetDevice()->runGarbageCollection();
        m_Device->GetDevice()->waitForIdle();

        m_EventBus.unsubscribe<FramebufferResizeEvent, &Renderer::OnFramebufferResized>(*this);

        m_3DRenderTarget = nullptr;

        m_ImGuiRenderer.reset();

        m_ImGuiGraphicsPipeline = nullptr;

        m_Backbuffers.clear();
        m_BackbufferDepthStencilTextures.clear();

        m_BindingSet = nullptr;
        m_BindingLayout = nullptr;
        m_Sampler = nullptr;

        for (auto& swapchain : m_SwapChainImages)
            swapchain.nvrhiHandle = nullptr;

        m_PSOCache->clear();
        m_ShaderCache->clear();

        m_Device->GetDevice()->runGarbageCollection();

        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        m_Device.reset();
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::BuildOffscreenFramebufferImages(OffscreenFramebuffer& fb, uint32_t width, uint32_t height) const
    {
        ZoneScoped;

        fb.m_Width = width;
        fb.m_Height = height;

        const nvrhi::TextureDesc colorTextureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName(fb.m_DebugName + " Color")
            .setFormat(fb.m_ColorFormat)
            .setWidth(width)
            .setHeight(height)
            .setInitialState(nvrhi::ResourceStates::ShaderResource)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        const nvrhi::TextureDesc depthStencilTextureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName(fb.m_DebugName + " Depth/Stencil")
            .setFormat(fb.m_DepthStencilFormat)
            .setWidth(width)
            .setHeight(height)
            .setInitialState(nvrhi::ResourceStates::DepthWrite)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            const nvrhi::TextureHandle colorTexture = m_Device->GetDevice()->createTexture(colorTextureDesc);
            fb.m_ColorImGuiTextures.push_back(m_ImGuiRenderer->AddTexture(colorTexture, m_Sampler));

            const nvrhi::TextureHandle depthStencilTexture = m_Device->GetDevice()->createTexture(depthStencilTextureDesc);
            fb.m_DepthStencilTextures.push_back(depthStencilTexture);

            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(colorTexture)
                .setDepthAttachment(depthStencilTexture);
            fb.m_Framebuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
        }

        // Transition color textures to ShaderResource
        const nvrhi::CommandListHandle initCommandList = m_Device->GetDevice()->createCommandList();
        initCommandList->open();
        for (auto& tex : fb.m_ColorImGuiTextures) {
            initCommandList->setTextureState(tex.texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
        }
        initCommandList->commitBarriers();
        initCommandList->close();
        m_Device->GetDevice()->executeCommandList(initCommandList);
    }

    std::unique_ptr<OffscreenFramebuffer> Renderer::CreateOffscreenFramebuffer(const OffscreenFramebufferDesc& desc) const
    {
        ZoneScoped;

        auto fb = std::make_unique<OffscreenFramebuffer>();
        fb->m_DebugName = desc.debugName;
        fb->m_ColorFormat = desc.colorFormat;
        fb->m_DepthStencilFormat = desc.depthStencilFormat;

        BuildOffscreenFramebufferImages(*fb, desc.width, desc.height);

        return fb;
    }

    void Renderer::ResizeOffscreenFramebuffer(OffscreenFramebuffer& fb, const uint32_t width, const uint32_t height) const
    {
        ZoneScoped;

        if (fb.m_Width == width && fb.m_Height == height)
            return;

        m_Device->GetDevice()->waitForIdle();

        // Release old resources
        for (auto& tex : fb.m_ColorImGuiTextures)
            m_ImGuiRenderer->RemoveTexture(tex);

        fb.m_ColorImGuiTextures.clear();
        fb.m_DepthStencilTextures.clear();
        fb.m_Framebuffers.clear();

        BuildOffscreenFramebufferImages(fb, width, height);
    }

    void Renderer::DestroyOffscreenFramebuffer(OffscreenFramebuffer& fb) const
    {
        ZoneScoped;

        m_Device->GetDevice()->waitForIdle();

        for (auto& tex : fb.m_ColorImGuiTextures)
            m_ImGuiRenderer->RemoveTexture(tex);

        fb.m_ColorImGuiTextures.clear();
        fb.m_DepthStencilTextures.clear();
        fb.m_Framebuffers.clear();
        fb.m_Width = 0;
        fb.m_Height = 0;
    }

    void Renderer::Set3DRenderTarget(OffscreenFramebuffer* target)
    {
        m_3DRenderTarget = target;
    }

    void Renderer::RequestOffscreenResize(OffscreenFramebuffer& fb, const uint32_t width, const uint32_t height)
    {
        if (fb.m_Width == width && fb.m_Height == height)
            return;

        for (auto& pending : m_PendingResizes) {
            if (pending.fb == &fb) {
                pending.width = width;
                pending.height = height;
                return;
            }
        }
        m_PendingResizes.push_back({ &fb, width, height });
    }

    void Renderer::FlushPendingResizes()
    {
        if (m_PendingResizes.empty())
            return;

        m_Device->GetDevice()->waitForIdle();

        for (auto& [fb, width, height] : m_PendingResizes)
            ResizeOffscreenFramebuffer(*fb, width, height);

        m_PendingResizes.clear();
    }

    void Renderer::OnFramebufferResized(FramebufferResizeEvent) {
        m_ForceResizeSwapchain = true;
    }

    void Renderer::BeginScene(const Camera& camera) {
        ZoneScoped;

        FlushPendingResizes();

        m_ImGuiRenderer->BeginFrame();

        if (!m_Window->IsMinimized())
            CREATE_BACKEND_SWITCH(BeginFrame);

        m_RenderingCommandList = m_Device->GetDevice()->createCommandList();
        m_RenderingCommandList->open();

        if (!m_Window->IsMinimized()) {
            // Clear the screen
            nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, m_Backbuffers[m_SwapChainIndex], 0, nvrhi::Color(1.0f, 1.0f, 1.0f, 1.0f));
            nvrhi::utils::ClearDepthStencilAttachment(m_RenderingCommandList, m_Backbuffers[m_SwapChainIndex], 1.0f, 0);
        }

        if (m_3DRenderTarget && m_3DRenderTarget->IsValid())
        {
            // Offscreen mode, render to the offscreen framebuffer
            const nvrhi::FramebufferHandle fb = m_3DRenderTarget->GetFramebuffer(m_SwapChainIndex);
            nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, fb, 0, nvrhi::Color(1, 1, 1, 1));
            nvrhi::utils::ClearDepthStencilAttachment(m_RenderingCommandList, fb, 1.0f, 0);

            m_ViewProjectionMatrix = camera.GetViewProjectionMatrix(m_3DRenderTarget->GetAspectRatio());
        } else if (!m_3DRenderTarget)
        {
            // Direct mode, render to swapchain/backbuffer
            int fbW, fbH;
            m_Window->GetFramebufferSize(&fbW, &fbH);
            fbW = glm::max(fbW, 1);
            fbH = glm::max(fbH, 1);
            const float aspectRatio = static_cast<float>(fbW) / static_cast<float>(fbH);
            m_ViewProjectionMatrix = camera.GetViewProjectionMatrix(aspectRatio);
        }
    }

    void Renderer::CheckInitializeRendered(Rendered& rendered, const nvrhi::FramebufferHandle& targetFB) const {
        if (!rendered.mesh->IsInitialized())
            rendered.mesh->Initialize(m_Device->GetDevice(), m_RenderingCommandList);

        if (!rendered.initialized) {
            ZoneScopedN("Initialize Rendered Buffers");

            const auto modelConstantsBufferDesc = nvrhi::utils::CreateVolatileConstantBufferDesc(sizeof(ModelConstants), "ModelConstants Buffer", g_MaxFramesInFlight * 2)
                .setInitialState(nvrhi::ResourceStates::ConstantBuffer)
                .setKeepInitialState(true);
            rendered.modelConstantsBuffer = m_Device->GetDevice()->createBuffer(modelConstantsBufferDesc);

            const auto bindingSetDesc = nvrhi::BindingSetDesc()
                .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, rendered.modelConstantsBuffer));

            ENGINE_ASSERT(nvrhi::utils::CreateBindingSetAndLayout(m_Device->GetDevice(), nvrhi::ShaderType::All, 0, bindingSetDesc, rendered.bindingLayout, rendered.bindingSet), "Failed to create Binding Set/Layout");
        }

        if (!rendered.initialized || rendered.material != rendered.lastUsedMaterial ||
                    !AreFramebuffersCompatible(targetFB->getFramebufferInfo(), rendered.lastUsedFramebuffer) ||
                    rendered.lastUsedCullBackfaces != rendered.cullBackfaces) {
            ZoneScopedN("Regenerate Graphics Pipeline");

            const nvrhi::RenderState renderState = nvrhi::RenderState()
                .setDepthStencilState(nvrhi::DepthStencilState()
                    .setDepthTestEnable(true)
                    .setDepthWriteEnable(true)
                    .setDepthFunc(nvrhi::ComparisonFunc::Less))
                .setRasterState(nvrhi::RasterState()
                    .setFrontCounterClockwise(true)
                    .setCullMode(rendered.cullBackfaces ? nvrhi::RasterCullMode::Back : nvrhi::RasterCullMode::None));

            PSOKey key{};
            key.vertexShader = rendered.material->vertexShader;
            key.fragmentShader = rendered.material->fragmentShader;
            key.renderState = renderState;
            key.primType = nvrhi::PrimitiveType::TriangleList;
            key.vertexAttributes = rendered.material->vertexAttributes;
            key.bindingLayout = rendered.bindingLayout;
            key.framebufferInfo = targetFB->getFramebufferInfo();

            rendered.graphicsPipeline = m_PSOCache->getPipeline(key);

            rendered.initialized = true;
            rendered.lastUsedCullBackfaces = rendered.cullBackfaces;
            rendered.lastUsedMaterial = rendered.material;
            rendered.lastUsedFramebuffer = targetFB->getFramebufferInfo();
        }
    }

    void Renderer::RenderRenderables(entt::registry& scene) const {
        ZoneScoped;

        nvrhi::FramebufferHandle targetFB;

        if (m_3DRenderTarget)
        {
            if (!m_3DRenderTarget->IsValid())
                return;
            targetFB = m_3DRenderTarget->GetFramebuffer(m_SwapChainIndex);
        } else
            targetFB = m_Backbuffers[m_SwapChainIndex];

        const auto view = scene.view<GameObject, Rendered>();

        for (const auto entity : view) {
            auto [gameObj, rendered] = view.get(entity);

            if (rendered.mesh == nullptr || rendered.material == nullptr)
                continue;

            CheckInitializeRendered(rendered, targetFB);

            ModelConstants modelConstants{};
            modelConstants.viewProj = m_ViewProjectionMatrix;
            modelConstants.model = gameObj.transform.GetMatrix();

            // if this is a performance hit, cache the result and only update if transform updates.
            modelConstants.normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelConstants.model)));

            m_RenderingCommandList->writeBuffer(rendered.modelConstantsBuffer, &modelConstants, sizeof(modelConstants));

            const auto graphicsState = nvrhi::GraphicsState()
                .setPipeline(rendered.graphicsPipeline)
                .setFramebuffer(targetFB)
                .addVertexBuffer(rendered.mesh->GetVertexBufferBinding())
                .setIndexBuffer(rendered.mesh->GetIndexBufferBinding())
                .addBindingSet(rendered.bindingSet)
                .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(
                    static_cast<float>(targetFB->getFramebufferInfo().width),
                    static_cast<float>(targetFB->getFramebufferInfo().height))));

            m_RenderingCommandList->setGraphicsState(graphicsState);

            nvrhi::DrawArguments drawArgs = nvrhi::DrawArguments()
                .setVertexCount(rendered.mesh->GetNumVertices());
            m_RenderingCommandList->drawIndexed(drawArgs);
        }
    }

    void Renderer::EndScene() {
        ZoneScoped;

        // Transition offscreen color texture to ShaderResource for ImGui sampling
        if (m_3DRenderTarget && m_3DRenderTarget->IsValid()) {
            const auto& tex = m_3DRenderTarget->GetImGuiTexture(m_SwapChainIndex);
            m_RenderingCommandList->setTextureState(tex.texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
            m_RenderingCommandList->commitBarriers();
        }

        int framebufferWidth, framebufferHeight;
        m_Window->GetFramebufferSize(&framebufferWidth, &framebufferHeight);

        framebufferWidth = glm::max(framebufferWidth, 1);
        framebufferHeight = glm::max(framebufferHeight, 1);

        const auto graphicsState = nvrhi::GraphicsState()
            .setPipeline(m_ImGuiGraphicsPipeline)
            .setFramebuffer(m_Backbuffers[m_SwapChainIndex])
            .addBindingSet(m_BindingSet);
        m_RenderingCommandList->setGraphicsState(graphicsState);

        m_ImGuiRenderer->Render(m_RenderingCommandList);

        m_RenderingCommandList->close();
        m_Device->GetDevice()->executeCommandList(m_RenderingCommandList);

        if (!m_Window->IsMinimized())
            CREATE_BACKEND_SWITCH(PresentFrame);

        m_Device->GetDevice()->runGarbageCollection();

        m_RenderingCommandList = nullptr;
    }

    void Renderer::GenerateBackbuffers() {
        ZoneScoped;

        m_Backbuffers.clear();
        m_BackbufferDepthStencilTextures.clear();

        const auto& firstImageDesc = m_SwapChainImages[0].nvrhiHandle->getDesc();

        const nvrhi::TextureDesc depthStencilDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName("Backbuffer Depth")
            .setFormat(g_DepthStencilFormat)
            .setWidth(firstImageDesc.width)
            .setHeight(firstImageDesc.height)
            .setInitialState(nvrhi::ResourceStates::DepthWrite)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        for (auto&[swapChainNvrhiHandle] : m_SwapChainImages) {
            auto depthStencilTexture = m_Device->GetDevice()->createTexture(depthStencilDesc);

            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(swapChainNvrhiHandle)
                .setDepthAttachment(depthStencilTexture);
            m_Backbuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
            m_BackbufferDepthStencilTextures.push_back(depthStencilTexture);
        }

        if (!m_HasImGuiGraphicsPipeline) {
            constexpr nvrhi::RenderState renderState = nvrhi::RenderState()
                    .setDepthStencilState(nvrhi::DepthStencilState()
                        .setDepthTestEnable(false));

            PSOKey key{};
            key.vertexShader       = g_ShaderEmptyVertex;
            key.fragmentShader     = g_ShaderEmptyFragment;
            key.renderState        = renderState;
            key.bindingLayout      = m_BindingLayout;
            key.framebufferInfo    = m_Backbuffers[0]->getFramebufferInfo();
            key.vertexAttributes   = g_ShaderEmptyAttributes;
            key.primType           = nvrhi::PrimitiveType::TriangleList;

            m_ImGuiGraphicsPipeline = m_PSOCache->getPipeline(key);
            m_HasImGuiGraphicsPipeline = true;
        }
    }
}
