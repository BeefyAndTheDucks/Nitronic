//
// Created by simon on 02/09/2025.
//

#include <iostream>
#include <fstream>

#include "renderer/Renderer.h"

#include <filesystem>

#include "core/Macros.h"
#include "nvrhi/utils.h"

NAMESPACE {

    static constexpr Vertex g_Vertices[] = {
        { { 0.f, 0.f, 0.f }, { 0.f, 0.f } },
        { { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
        { { 0.5f, 0.5f, 0.f }, { 0.5f, 0.5f } },
    };

    static std::vector<char> readFile(const std::filesystem::path& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::cout << std::filesystem::current_path() << std::endl;
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    Renderer::Renderer(const RenderingBackend backend, Window* window)
        : m_Backend(backend), m_Window(window)
    {
        std::cout << "Using backend " << RenderingBackendToString(backend) << std::endl;

        CREATE_BACKEND_SWITCH(Init);
        m_Device = new Device(m_Backend, m_RendererData);
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);

        for (const auto& img : m_SwapChainImages) {
            nvrhi::FramebufferDesc fbDesc;
            fbDesc.addColorAttachment(img.nvrhiHandle);
        }

        auto vertShaderCode = readFile("../assets/shaders/basic.vert.spirv");
        auto fragShaderCode = readFile("../assets/shaders/basic.frag.spirv");

        std::cout << "Shader code size: " << vertShaderCode.size() << std::endl;

        nvrhi::ShaderHandle vertexShader = m_Device->GetDevice()->createShader(
            nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Vertex),
            vertShaderCode.data(), vertShaderCode.size());

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

        nvrhi::InputLayoutHandle inputLayout = m_Device->GetDevice()->createInputLayout(
            attributes, uint32_t(std::size(attributes)), vertexShader);

        nvrhi::ShaderHandle pixelShader = m_Device->GetDevice()->createShader(
            nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Pixel),
            fragShaderCode.data(), fragShaderCode.size());

        for (auto& swapChainImage : m_SwapChainImages) {
            auto framebufferDesc = nvrhi::FramebufferDesc()
            .addColorAttachment(swapChainImage.nvrhiHandle);

            m_Framebuffers.push_back(m_Device->GetDevice()->createFramebuffer(framebufferDesc));
        }

        auto layoutDesc = nvrhi::BindingLayoutDesc()
            .setVisibility(nvrhi::ShaderType::All);
        nvrhi::BindingLayoutHandle bindingLayout = m_Device->GetDevice()->createBindingLayout(layoutDesc);

        auto vertexBufferDesc = nvrhi::BufferDesc()
            .setByteSize(sizeof(g_Vertices))
            .setIsVertexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
            .setKeepInitialState(true) // enable fully automatic state tracking
            .setDebugName("Vertex Buffer");

        nvrhi::BufferHandle vertexBuffer = m_Device->GetDevice()->createBuffer(vertexBufferDesc);

        nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        // Note: the binding set must include all bindings declared in the layout, and nothing else.
        // This condition is tested by the validation layer.
        // The order of items in the binding set doesn't matter.
        auto bindingSetDesc = nvrhi::BindingSetDesc();

        nvrhi::BindingSetHandle bindingSet = m_Device->GetDevice()->createBindingSet(bindingSetDesc, bindingLayout);

        commandList->open();

        commandList->writeBuffer(vertexBuffer, g_Vertices, sizeof(g_Vertices));

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);
        m_Device->GetDevice()->runGarbageCollection();
    }

    Renderer::~Renderer() {
        for (auto& frameBuffer : m_Framebuffers)
            frameBuffer = nullptr;

        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        CREATE_BACKEND_SWITCH(BeginFrame); // not sure if required. maybe prerender and postrender?

        nvrhi::CommandListHandle commandList = m_Device->GetDevice()->createCommandList();

        commandList->open();

        nvrhi::utils::ClearColorAttachment(commandList, m_Framebuffers[m_SwapChainIndex], 0, nvrhi::Color(1, 0, 0, 1));

        commandList->close();
        m_Device->GetDevice()->executeCommandList(commandList);
        m_Device->GetDevice()->runGarbageCollection();

        CREATE_BACKEND_SWITCH(PresentFrame);
    }
}
