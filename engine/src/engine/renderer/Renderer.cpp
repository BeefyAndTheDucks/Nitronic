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

        auto framebufferDesc = nvrhi::FramebufferDesc()
            .addColorAttachment(m_SwapChainImages[0].nvrhiHandle); // you can specify a particular subresource if necessary

        m_Framebuffer = m_Device->GetDevice()->createFramebuffer(framebufferDesc);

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

        m_CommandList = m_Device->GetDevice()->createCommandList();

        // Note: the binding set must include all bindings declared in the layout, and nothing else.
        // This condition is tested by the validation layer.
        // The order of items in the binding set doesn't matter.
        auto bindingSetDesc = nvrhi::BindingSetDesc();

        nvrhi::BindingSetHandle bindingSet = m_Device->GetDevice()->createBindingSet(bindingSetDesc, bindingLayout);

        m_CommandList->open();

        m_CommandList->writeBuffer(vertexBuffer, g_Vertices, sizeof(g_Vertices));

        m_CommandList->close();
        m_Device->GetDevice()->executeCommandList(m_CommandList);
    }

    Renderer::~Renderer() {
        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        CREATE_BACKEND_SWITCH(Render, deltaTime); // not sure if required. maybe prerender and postrender?

        m_CommandList->open();

        nvrhi::utils::ClearColorAttachment(m_CommandList, m_Framebuffer, 0, nvrhi::Color(1, 0, 0, 1));

        m_CommandList->close();
        m_Device->GetDevice()->executeCommandList(m_CommandList);
    }
}
