//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <fstream>

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
        : m_Backend(backend), m_RendererData(nullptr), m_Window(window)
    {
        std::cout << "Using " << RenderingBackendToString(backend) << " backend." << std::endl;

        CREATE_BACKEND_SWITCH(Init);
        m_Device = new Device(m_Backend, m_RendererData);
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);

        m_ShaderCache.emplace(
            m_Device->GetDevice(),
            g_ShadersDirectory,
            backend
        );

        m_PSOCache.emplace(m_Device->GetDevice(), *m_ShaderCache);

        nvrhi::VertexAttributeDesc attributes[] = {
            nvrhi::VertexAttributeDesc()
                .setName("a_Position")
                .setFormat(nvrhi::Format::RGB32_FLOAT)
                .setOffset(offsetof(Vertex, position))
                .setElementStride(sizeof(Vertex)),
            nvrhi::VertexAttributeDesc()
                .setName("a_TexCoord")
                .setFormat(nvrhi::Format::RG32_FLOAT)
                .setOffset(offsetof(Vertex, texCoord))
                .setElementStride(sizeof(Vertex)),
        };

        m_InputLayout = m_Device->GetDevice()->createInputLayout(
            attributes, std::size(attributes), m_ShaderCache->getShader(g_ShaderEmptyVertex));

        // Create buffers
        auto vertexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(sizeof(Vertex))
            .setIsVertexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Vertex Buffer");
        m_VertexBuffer = m_Device->GetDevice()->createBuffer(vertexBufferDesc);

        auto indexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(sizeof(uint32_t))
            .setIsIndexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::IndexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Index Buffer");
        m_IndexBuffer = m_Device->GetDevice()->createBuffer(indexBufferDesc);

        if (!nvrhi::utils::CreateBindingSetAndLayout(m_Device->GetDevice(), nvrhi::ShaderType::All, 0, nvrhi::BindingSetDesc(), m_BindingLayout, m_BindingSet)) {
            throw std::runtime_error("Failed to create binding set and layout.");
        }

        GenerateBackbuffers();

        nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        commandList->open();

        Vertex vertices[] = { {0.f,  0.f, 0.f, 0.f, 0.f} };
        uint32_t indices[] = { 0 };

        commandList->writeBuffer(m_VertexBuffer, vertices, sizeof(Vertex));
        commandList->writeBuffer(m_IndexBuffer, indices, sizeof(uint32_t));

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);

        m_ImGuiRenderer = new ImGuiRenderer(backend, window, m_RendererData, m_Device->GetDeviceData(), m_Backbuffers[m_SwapChainIndex]);

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

        delete m_ImGuiRenderer;

        m_ImGuiGraphicsPipeline = nullptr;

        m_Framebuffers.clear();
        m_Backbuffers.clear();
        m_ImGuiFramebufferColorTextures.clear();
        m_ImGuiFramebufferDepthTextures.clear();

        m_BindingSet = nullptr;

        m_BindingLayout = nullptr;
        m_InputLayout = nullptr;

        m_VertexBuffer = nullptr;
        m_IndexBuffer = nullptr;
        m_Sampler = nullptr;

        for (auto& swapchain : m_SwapChainImages)
            swapchain.nvrhiHandle = nullptr;

        m_PSOCache->clear();
        m_ShaderCache->clear();

        m_Device->GetDevice()->runGarbageCollection();

        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::BeginScene(const Camera& camera) {
        if (m_Window->IsMinimized())
            return; // Skip rendering.

        m_ImGuiRenderer->BeginFrame();

        CREATE_BACKEND_SWITCH(BeginFrame); // not sure if required. maybe prerender and postrender?

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

        // Clear the screen
        nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, m_Backbuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));

        if (m_ValidGameWindow) {
            const float gameWindowAspect = m_FramebufferWidth / m_FramebufferHeight;

            nvrhi::utils::ClearColorAttachment(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));
            nvrhi::utils::ClearDepthStencilAttachment(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], 1.0f, 0);

            m_ViewProjectionMatrix = camera.GetViewProjectionMatrix(gameWindowAspect);
        }
    }

    void Renderer::RenderModel(Model* model) {
        if (!m_ValidGameWindow)
            return;

        if (!model->IsInitialized())
            model->Initialize(m_Device->GetDevice(), m_RenderingCommandList, m_PSOCache.value(), m_Framebuffers[m_SwapChainIndex]);
        model->Render(m_RenderingCommandList, m_Framebuffers[m_SwapChainIndex], m_ViewProjectionMatrix);
    }

    void Renderer::EndScene() {
        m_RenderingCommandList->setTextureState(m_ImGuiFramebufferColorTextures[m_SwapChainIndex].texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);

        int framebufferWidth, framebufferHeight;
        m_Window->GetFramebufferSize(&framebufferWidth, &framebufferHeight);

        const auto graphicsState = nvrhi::GraphicsState()
            .setPipeline(m_ImGuiGraphicsPipeline)
            .setFramebuffer(m_Backbuffers[m_SwapChainIndex])
            .addVertexBuffer(nvrhi::VertexBufferBinding(m_VertexBuffer))
            .setIndexBuffer(nvrhi::IndexBufferBinding(m_IndexBuffer))
            .addBindingSet(m_BindingSet)
            .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight))));
        m_RenderingCommandList->setGraphicsState(graphicsState);

        m_ImGuiRenderer->Render(m_RenderingCommandList);

        m_RenderingCommandList->close();
        m_Device->GetDevice()->executeCommandList(m_RenderingCommandList);

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
