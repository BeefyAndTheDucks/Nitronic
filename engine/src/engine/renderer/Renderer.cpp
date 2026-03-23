//
// Created by simon on 02/09/2025.
//

#include <iostream>

#include "renderer/Renderer.h"

#include <filesystem>

#include "imgui.h"
#include "core/Macros.h"
#include "nvrhi/utils.h"
#include "renderer/Camera.h"
#include "renderer/Constants.h"
#include "renderer/ImGuiRenderer.h"
#include "renderer/Shaders.h"

NAMESPACE {

    Renderer::Renderer(const RenderingBackend backend, Window* window)
        : m_Backend(backend), m_Window(window), m_RendererData(nullptr), m_ViewProjectionMatrix(glm::identity<glm::mat4>())
    {
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

        m_ImGuiRenderer = std::make_unique<ImGuiRenderer>(backend, window, m_RendererData.get(), m_Device->GetDeviceData());

        const nvrhi::SamplerDesc samplerDesc = nvrhi::SamplerDesc()
            .setAllAddressModes(nvrhi::SamplerAddressMode::Repeat)
            .setMaxAnisotropy(1.0f);
        m_Sampler = m_Device->GetDevice()->createSampler(samplerDesc);
    }

    Renderer::~Renderer() {
        m_Device->GetDevice()->runGarbageCollection();
        m_Device->GetDevice()->waitForIdle();

        if (m_HasGeneratedImGuiFramebuffer)
            for (ImGuiTexture& texture : m_ImGuiFramebufferColorTextures)
                m_ImGuiRenderer->RemoveTexture(texture);

        m_ImGuiRenderer.reset();

        m_ImGuiGraphicsPipeline = nullptr;

        m_Framebuffers.clear();
        m_Backbuffers.clear();
        m_ImGuiFramebufferColorTextures.clear();
        m_ImGuiFramebufferDepthTextures.clear();

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

    void Renderer::BeginScene(const Camera& camera) {
        m_ImGuiRenderer->BeginFrame();

        if (!m_Window->IsMinimized())
            CREATE_BACKEND_SWITCH(BeginFrame);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game");
        const ImVec2 windowSize = ImGui::GetContentRegionAvail();
        m_ValidGameWindow = windowSize.x > 0 && windowSize.y > 0;
        if (m_ValidGameWindow) {
            if (windowSize.x != m_FramebufferWidth || windowSize.y != m_FramebufferHeight)
                RegenerateFrambuffers(windowSize.x, windowSize.y);
            ImGui::Image(m_ImGuiFramebufferColorTextures[m_SwapChainIndex].textureID, windowSize);
        }
        ImGui::End();
        ImGui::PopStyleVar();

        m_RenderingCommandList = m_Device->GetDevice()->createCommandList();
        m_RenderingCommandList->open();

        if (!m_Window->IsMinimized()) {
            // Clear the screen
            nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, m_Backbuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));
        }

        if (m_ValidGameWindow) {
            const float gameWindowAspect = m_FramebufferWidth / m_FramebufferHeight;

            nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));
            nvrhi::utils::ClearDepthStencilAttachment(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], 1.0f, 0);

            m_ViewProjectionMatrix = camera.GetViewProjectionMatrix(gameWindowAspect);
        }
    }

    void Renderer::RenderModel(Model& model) {
        if (!m_ValidGameWindow)
            return;

        if (!model.IsInitialized())
            model.Initialize(m_Device->GetDevice(), m_RenderingCommandList, *m_PSOCache, m_Framebuffers[m_SwapChainIndex]);
        model.Render(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], m_ViewProjectionMatrix);
    }

    void Renderer::EndScene() {
        m_RenderingCommandList->setTextureState(m_ImGuiFramebufferColorTextures[m_SwapChainIndex].texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);

        int framebufferWidth, framebufferHeight;
        m_Window->GetFramebufferSize(&framebufferWidth, &framebufferHeight);

        framebufferWidth = std::max(framebufferWidth, 1);
        framebufferHeight = std::max(framebufferHeight, 1);

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

    void Renderer::RegenerateFrambuffers(const float requestedWidth, const float requestedHeight) {
        m_Device->GetDevice()->waitForIdle();

        m_FramebufferWidth = requestedWidth;
        m_FramebufferHeight = requestedHeight;

        if (m_HasGeneratedImGuiFramebuffer)
            for (ImGuiTexture& texture : m_ImGuiFramebufferColorTextures)
                m_ImGuiRenderer->RemoveTexture(texture);

        m_ImGuiFramebufferColorTextures.clear();
        m_ImGuiFramebufferDepthTextures.clear();
        m_Framebuffers.clear();

        // same number of images as the swapchain
        const nvrhi::TextureDesc colorTextureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName("ImGui Window Render Target (Color)")
            .setFormat(nvrhi::Format::SBGRA8_UNORM)
            .setWidth(static_cast<uint32_t>(m_FramebufferWidth))
            .setHeight(static_cast<uint32_t>(m_FramebufferHeight))
            .setInitialState(nvrhi::ResourceStates::ShaderResource)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        const nvrhi::TextureDesc depthTextureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName("ImGui Window Render Target (Depth)")
            .setFormat(nvrhi::Format::D32) // D24S8 if I need stencil
            .setWidth(static_cast<uint32_t>(m_FramebufferWidth))
            .setHeight(static_cast<uint32_t>(m_FramebufferHeight))
            .setInitialState(nvrhi::ResourceStates::DepthWrite)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        for (int i = 0; i < m_SwapChainImages.size(); i++) {
            const nvrhi::TextureHandle colorTexture = m_Device->GetDevice()->createTexture(colorTextureDesc);
            m_ImGuiFramebufferColorTextures.push_back(m_ImGuiRenderer->AddTexture(colorTexture, m_Sampler));

            const nvrhi::TextureHandle depthTexture = m_Device->GetDevice()->createTexture(depthTextureDesc);
            m_ImGuiFramebufferDepthTextures.push_back(depthTexture);

            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(colorTexture)
                .setDepthAttachment(depthTexture);
            auto framebuffer = m_Device->GetDevice()->createFramebuffer(framebufferDesc);
            m_Framebuffers.push_back(framebuffer);
        }

        const nvrhi::CommandListHandle initCommandList = m_Device->GetDevice()->createCommandList();
        initCommandList->open();
        for (auto& tex : m_ImGuiFramebufferColorTextures) {
            initCommandList->setTextureState(tex.texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
        }

        initCommandList->commitBarriers();
        initCommandList->close();
        m_Device->GetDevice()->executeCommandList(initCommandList);

        m_HasGeneratedImGuiFramebuffer = true;
    }

    void Renderer::GenerateBackbuffers() {
        m_Backbuffers.clear();

        for (auto&[swapChainNvrhiHandle] : m_SwapChainImages) {
            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(swapChainNvrhiHandle);
            m_Backbuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
        }

        if (!m_HasImGuiGraphicsPipeline) {
            constexpr nvrhi::RenderState renderState = nvrhi::RenderState()
                    .setDepthStencilState(nvrhi::DepthStencilState()
                        .setDepthTestEnable(false)
                        .setStencilEnable(false));

            PSOKey key{};
            key.vertexShader       = g_ShaderEmptyVertex;
            key.fragmentShader     = g_ShaderEmptyFragment;
            key.renderState        = renderState;
            key.bindingLayout      = m_BindingLayout;
            key.framebufferInfo    = m_Backbuffers[0]->getFramebufferInfo();
            key.vertexAttributes   = g_ShaderEmptyAttributes;
            key.primType           = nvrhi::PrimitiveType::TriangleList;

            m_ImGuiGraphicsPipeline = m_PSOCache->get(key);
            m_HasImGuiGraphicsPipeline = true;
        }
    }
}
