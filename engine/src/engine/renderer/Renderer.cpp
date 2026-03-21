//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <fstream>

#include "renderer/Renderer.h"

#include <filesystem>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "imgui.h"
#include "core/Macros.h"
#include "nvrhi/utils.h"
#include "renderer/ImGuiRenderer.h"

NAMESPACE {

    static const Vertex g_Vertices[] = {
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} }, // front face
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

        { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} }, // right side face
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} },

        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // left side face
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

        { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // back face
        { {-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },

        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f} }, // top face
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} },

        { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} }, // bottom face
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
    };

    static const uint32_t g_Indices[] = {
        0,  1,  2,   0,  3,  1, // front face
        4,  5,  6,   4,  7,  5, // left face
        8,  9, 10,   8, 11,  9, // right face
       12, 13, 14,  12, 15, 13, // back face
       16, 17, 18,  16, 19, 17, // top face
       20, 21, 22,  20, 23, 21, // bottom face
    };

    Renderer::Renderer(const RenderingBackend backend, Window* window)
        : m_Backend(backend), m_RendererData(nullptr), m_Window(window), m_HasGeneratedImGuiFramebuffer(false)
    {
        std::cout << "Using " << RenderingBackendToString(backend) << " backend." << std::endl;

        CREATE_BACKEND_SWITCH(Init);
        m_Device = new Device(m_Backend, m_RendererData);
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);

        m_VertexShader = LoadShader("Basic", ShaderType::Vertex);
        m_FragmentShader = LoadShader("Basic", ShaderType::Fragment);

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
            attributes, std::size(attributes), m_VertexShader);

        // Create buffers
        auto vertexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(sizeof(g_Vertices))
            .setIsVertexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Vertex Buffer");
        m_VertexBuffer = m_Device->GetDevice()->createBuffer(vertexBufferDesc);

        auto indexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(sizeof(g_Indices))
            .setIsIndexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::IndexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Index Buffer");
        m_IndexBuffer = m_Device->GetDevice()->createBuffer(indexBufferDesc);

        auto frameConstantsBufferDesc = nvrhi::utils::CreateStaticConstantBufferDesc(sizeof(FrameConstants), "FrameConstantsBuffer")
            .setInitialState(nvrhi::ResourceStates::ConstantBuffer)
            .setKeepInitialState(true);
        m_FrameConstantsBuffer = m_Device->GetDevice()->createBuffer(frameConstantsBufferDesc);

        auto bindingSetDesc = nvrhi::BindingSetDesc()
            .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_FrameConstantsBuffer));

        if (!nvrhi::utils::CreateBindingSetAndLayout(m_Device->GetDevice(), nvrhi::ShaderType::All, 0, bindingSetDesc, m_BindingLayout, m_BindingSet)) {
            throw std::runtime_error("Failed to create binding set and layout.");
        }

        GenerateBackbuffers();

        nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        commandList->open();

        commandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));
        commandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);
        m_Device->GetDevice()->runGarbageCollection();

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
            for (ImGuiTexture& texture : m_ImGuiFramebufferTextures)
                m_ImGuiRenderer->RemoveTexture(texture);

        delete m_ImGuiRenderer;

        m_ImGuiGraphicsStates.clear();
        m_GraphicsStates.clear();

        m_ImGuiGraphicsPipelines.clear();
        m_GraphicsPipelines.clear();

        m_Framebuffers.clear();
        m_Backbuffers.clear();
        m_ImGuiFramebufferTextures.clear();

        m_BindingSet = nullptr;

        m_BindingLayout = nullptr;
        m_InputLayout = nullptr;

        m_VertexShader = nullptr;
        m_FragmentShader = nullptr;

        m_VertexBuffer = nullptr;
        m_IndexBuffer = nullptr;
        m_FrameConstantsBuffer = nullptr;
        m_Sampler = nullptr;

        for (auto& swapchain : m_SwapChainImages)
            swapchain.nvrhiHandle = nullptr;

        m_Device->GetDevice()->runGarbageCollection();

        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        if (m_Window->IsMinimized())
            return; // Skip rendering.

        m_ImGuiRenderer->BeginFrame();

        m_TimePassed += deltaTime;

        CREATE_BACKEND_SWITCH(BeginFrame); // not sure if required. maybe prerender and postrender?

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game");
        const ImVec2 windowSize = ImGui::GetContentRegionAvail();
        const bool validWindow = windowSize.x > 0 && windowSize.y > 0;
        if (validWindow) {
            if (windowSize.x != m_FramebufferWidth || windowSize.y != m_FramebufferHeight)
                RegenerateFrambuffers(windowSize.x, windowSize.y);
            ImGui::Image(m_ImGuiFramebufferTextures[m_SwapChainIndex].textureID, windowSize);
        }
        ImGui::End();
        ImGui::PopStyleVar();

        //const float gameWindowAspect = m_FramebufferWidth / m_FramebufferHeight;

        const nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();
        commandList->open();

        // Clear the screen
        nvrhi::utils::ClearColorAttachment(commandList, m_Backbuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));

        if (validWindow) {
            nvrhi::utils::ClearColorAttachment(commandList, m_Framebuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));

            commandList->setGraphicsState(m_GraphicsStates[m_SwapChainIndex]);

            nvrhi::DrawArguments drawArgs{};
            drawArgs.vertexCount = static_cast<uint32_t>(std::size(g_Indices));
            commandList->drawIndexed(drawArgs);
        }

        commandList->setTextureState(m_ImGuiFramebufferTextures[m_SwapChainIndex].texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);

        commandList->setGraphicsState(m_ImGuiGraphicsStates[m_SwapChainIndex]);

        m_ImGuiRenderer->Render(commandList);

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);

        CREATE_BACKEND_SWITCH(PresentFrame);

        m_Device->GetDevice()->runGarbageCollection();
    }

    void Renderer::RegenerateFrambuffers(const float requestedWidth, const float requestedHeight) {
        m_Device->GetDevice()->waitForIdle();

        m_FramebufferWidth = requestedWidth;
        m_FramebufferHeight = requestedHeight;

        if (m_HasGeneratedImGuiFramebuffer)
            for (ImGuiTexture& texture : m_ImGuiFramebufferTextures)
                m_ImGuiRenderer->RemoveTexture(texture);

        m_ImGuiFramebufferTextures.clear();
        m_Framebuffers.clear();
        m_GraphicsPipelines.clear();
        m_GraphicsStates.clear();

        // same number of images as the swapchain
        const nvrhi::TextureDesc textureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setDebugName("ImGui Window Render Target")
            .setFormat(nvrhi::Format::SBGRA8_UNORM)
            .setWidth(static_cast<uint32_t>(m_FramebufferWidth))
            .setHeight(static_cast<uint32_t>(m_FramebufferHeight))
            .setInitialState(nvrhi::ResourceStates::ShaderResource)
            .setKeepInitialState(true)
            .setIsRenderTarget(true);

        for (int i = 0; i < m_SwapChainImages.size(); i++) {
            const nvrhi::TextureHandle texture = m_Device->GetDevice()->createTexture(textureDesc);
            m_ImGuiFramebufferTextures.push_back(m_ImGuiRenderer->AddTexture(texture, m_Sampler));

            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(texture);
            auto framebuffer = m_Device->GetDevice()->createFramebuffer(framebufferDesc);
            m_Framebuffers.push_back(framebuffer);

            nvrhi::GraphicsPipelineDesc pipelineDesc = nvrhi::GraphicsPipelineDesc()
                .setInputLayout(m_InputLayout)
                .addBindingLayout(m_BindingLayout)
                .setVertexShader(m_VertexShader)
                .setPixelShader(m_FragmentShader)
                .setPrimType(nvrhi::PrimitiveType::TriangleList)
                .setRenderState(nvrhi::RenderState()
                    .setDepthStencilState(nvrhi::DepthStencilState()
                        .setDepthTestEnable(false)
                        .setStencilEnable(false))
                    .setRasterState(nvrhi::RasterState()
                        .setFrontCounterClockwise(true)));

            auto pipeline = m_Device->GetDevice()->createGraphicsPipeline(pipelineDesc, framebuffer);
            m_GraphicsPipelines.push_back(pipeline);

            auto graphicsState = nvrhi::GraphicsState()
                .setPipeline(pipeline)
                .setFramebuffer(framebuffer)
                .addVertexBuffer(nvrhi::VertexBufferBinding(m_VertexBuffer))
                .setIndexBuffer(nvrhi::IndexBufferBinding(m_IndexBuffer))
                .addBindingSet(m_BindingSet)
                .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(m_FramebufferWidth, m_FramebufferHeight)));
            m_GraphicsStates.push_back(graphicsState);
        }

        nvrhi::CommandListHandle initCommandList = m_Device->GetDevice()->createCommandList();
        initCommandList->open();
        for (auto& tex : m_ImGuiFramebufferTextures) {
            initCommandList->setTextureState(tex.texture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
        }

        FrameConstants cpuFrameConstants{};
        cpuFrameConstants.view          = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
        cpuFrameConstants.projection    = glm::perspective(glm::radians(45.0f), m_FramebufferWidth / m_FramebufferHeight, 0.1f, 100.0f);
        initCommandList->writeBuffer(m_FrameConstantsBuffer, &cpuFrameConstants, sizeof(cpuFrameConstants));

        initCommandList->commitBarriers();
        initCommandList->close();
        m_Device->GetDevice()->executeCommandList(initCommandList);

        m_HasGeneratedImGuiFramebuffer = true;
    }

    void Renderer::GenerateBackbuffers() {
        m_Backbuffers.clear();
        m_ImGuiGraphicsPipelines.clear();
        m_ImGuiGraphicsStates.clear();

        for (auto&[swapChainNvrhiHandle] : m_SwapChainImages) {
            auto framebufferDesc = nvrhi::FramebufferDesc()
                .addColorAttachment(swapChainNvrhiHandle);
            m_Backbuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
        }

        for (auto& backbuffer : m_Backbuffers) {
            nvrhi::GraphicsPipelineDesc pipelineDesc = nvrhi::GraphicsPipelineDesc()
                .setInputLayout(m_InputLayout)
                .addBindingLayout(m_BindingLayout)
                .setVertexShader(m_VertexShader)
                .setPixelShader(m_FragmentShader)
                .setPrimType(nvrhi::PrimitiveType::TriangleList)
                .setRenderState(nvrhi::RenderState()
                    .setDepthStencilState(nvrhi::DepthStencilState()
                        .setDepthTestEnable(false)
                        .setStencilEnable(false)))
                //    .setRasterState(nvrhi::RasterState()
                //        .setFrontCounterClockwise(true)))
            ;

            auto pipeline = m_Device->GetDevice()->createGraphicsPipeline(pipelineDesc, backbuffer);
            m_ImGuiGraphicsPipelines.push_back(pipeline);

            int framebufferWidth, framebufferHeight;
            m_Window->GetFramebufferSize(&framebufferWidth, &framebufferHeight);

            auto graphicsState = nvrhi::GraphicsState()
                .setPipeline(pipeline)
                .setFramebuffer(backbuffer)
                .addVertexBuffer(nvrhi::VertexBufferBinding(m_VertexBuffer))
                .setIndexBuffer(nvrhi::IndexBufferBinding(m_IndexBuffer))
                .addBindingSet(m_BindingSet)
                .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight))));
            m_ImGuiGraphicsStates.push_back(graphicsState);
        }
    }
}
