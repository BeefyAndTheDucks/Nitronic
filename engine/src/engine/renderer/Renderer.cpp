//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <fstream>

#include "renderer/Renderer.h"

#include <filesystem>

#include "core/Macros.h"
#include "nvrhi/utils.h"
#include "util/IOUtils.h"

NAMESPACE {

    static constexpr Vertex g_Vertices[] = {
        { { -0.5f, -0.5f, 0.f }, { 0.f, 0.f } },
        { {  0.5f, -0.5f, 0.f }, { 1.f, 0.f } },
        { {  0.5f,  0.5f, 0.f }, { 1.f, 1.f } },
        { { -0.5f,  0.5f, 0.f }, { 0.f, 1.f } },
    };

    static constexpr uint32_t g_Indices[] = { 0, 1, 2, 2, 3, 0 };

    Renderer::Renderer(const RenderingBackend backend, Window* window)
        : m_Backend(backend), m_Window(window), m_RendererData(nullptr)
    {
        std::cout << "Using " << RenderingBackendToString(backend) << " backend." << std::endl;

        CREATE_BACKEND_SWITCH(Init);
        m_Device = new Device(m_Backend, m_RendererData);
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);

        auto vertShaderCode = LoadShaderCode("Basic", ShaderType::Vertex);
        auto fragShaderCode = LoadShaderCode("Basic", ShaderType::Fragment);

        m_VertexShader = m_Device->GetDevice()->createShader(
            nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Vertex),
            vertShaderCode.data(), vertShaderCode.size());

        m_PixelShader = m_Device->GetDevice()->createShader(
            nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Pixel),
            fragShaderCode.data(), fragShaderCode.size());

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

#pragma warning(push)
#pragma warning(disable: 4267)
        m_InputLayout = m_Device->GetDevice()->createInputLayout(
            attributes, std::size(attributes), m_VertexShader);
#pragma warning(pop)

        auto layoutDesc = nvrhi::BindingLayoutDesc()
            .setVisibility(nvrhi::ShaderType::All);
        m_BindingLayout = m_Device->GetDevice()->createBindingLayout(layoutDesc);

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

        nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        // Note: the binding set must include all bindings declared in the layout, and nothing else.
        // This condition is tested by the validation layer.
        // The order of items in the binding set doesn't matter.
        auto bindingSetDesc = nvrhi::BindingSetDesc();
        //bindingSetDesc.addItem();

        m_BindingSet = m_Device->GetDevice()->createBindingSet(bindingSetDesc, m_BindingLayout);

        GenerateFramebuffers();

        commandList->open();

        commandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));
        commandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);
        m_Device->GetDevice()->runGarbageCollection();
    }

    Renderer::~Renderer() {
        m_Device->GetDevice()->runGarbageCollection();
        m_Device->GetDevice()->waitForIdle();

        for (auto& pipeline : m_GraphicsPipelines)
            pipeline = nullptr;

        for (auto& frameBuffer : m_Framebuffers)
            frameBuffer = nullptr;

        for (auto& swapchain : m_SwapChainImages)
            swapchain.nvrhiHandle = nullptr;

        m_VertexBuffer = nullptr;
        m_IndexBuffer = nullptr;
        m_BindingSet = nullptr;
        m_InputLayout = nullptr;
        m_VertexShader = nullptr;
        m_PixelShader = nullptr;
        m_BindingLayout = nullptr;

        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        if (m_Window->IsMinimized())
            return; // Skip rendering.

        CREATE_BACKEND_SWITCH(BeginFrame); // not sure if required. maybe prerender and postrender?

        const nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        commandList->open();

        nvrhi::utils::ClearColorAttachment(commandList, m_Framebuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 1, 1, 1));

        commandList->setGraphicsState(m_GraphicsStates[m_SwapChainIndex]);

        nvrhi::DrawArguments drawArgs{};
        drawArgs.vertexCount = static_cast<uint32_t>(std::size(g_Indices));
        commandList->drawIndexed(drawArgs);

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);
        m_Device->GetDevice()->runGarbageCollection();

        CREATE_BACKEND_SWITCH(PresentFrame);
    }

    void Renderer::GenerateFramebuffers() {
        m_Framebuffers.clear();
        m_GraphicsPipelines.clear();
        m_GraphicsStates.clear();

        for (auto&[swapChainNvrhiHandle] : m_SwapChainImages) {
            auto framebufferDesc = nvrhi::FramebufferDesc()
            .addColorAttachment(swapChainNvrhiHandle);

            m_Framebuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
        }

        for (const auto& framebuffer : m_Framebuffers) {
            nvrhi::GraphicsPipelineDesc pipelineDesc;
            pipelineDesc.inputLayout = m_InputLayout;
            pipelineDesc.VS = m_VertexShader;
            pipelineDesc.PS = m_PixelShader;
            pipelineDesc.bindingLayouts = { m_BindingLayout };
            pipelineDesc.renderState.rasterState.frontCounterClockwise = true;
            pipelineDesc.renderState.depthStencilState.depthTestEnable = false;
            pipelineDesc.renderState.depthStencilState.stencilEnable = false;

            auto pipeline = m_Device->GetDevice()->createGraphicsPipeline(pipelineDesc, framebuffer);
            m_GraphicsPipelines.push_back(pipeline);

            int framebufferWidth, framebufferHeight;
            m_Window->GetFramebufferSize(&framebufferWidth, &framebufferHeight);

            nvrhi::GraphicsState graphicsState = nvrhi::GraphicsState()
                .setPipeline(pipeline)
                .setFramebuffer(framebuffer)
                .addVertexBuffer(nvrhi::VertexBufferBinding(m_VertexBuffer))
                .setIndexBuffer(nvrhi::IndexBufferBinding(m_IndexBuffer))
                .addBindingSet(m_BindingSet)
                .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight))));
            m_GraphicsStates.push_back(graphicsState);
        }
    }
}
