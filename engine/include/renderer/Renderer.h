//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include <filesystem>
#include <iostream>
#include <queue>

#include "core/Enums.h"
#include "core/Macros.h"

#include "Device.h"
#include "engine/Window.h"
#include "util/IOUtils.h"

NAMESPACE {

    constexpr size_t g_MaxFramesInFlight = 2;

    constexpr auto g_ShadersDirectory = "../shaders/";

    struct SwapChainImage {
        //vk::Image image;
        nvrhi::TextureHandle nvrhiHandle;
    };

    struct Vertex {
        float position[3];
        float texCoord[2];
    };

    struct NvrhiMessageCallback : nvrhi::IMessageCallback
    {
        static NvrhiMessageCallback& GetInstance()
        {
            static NvrhiMessageCallback s_Instance;
            return s_Instance;
        }

        void message(nvrhi::MessageSeverity severity, const char* messageText) override {
            std::cout << "NVRHI: " << messageText << std::endl;
        }
    };

    class Renderer {
    public:
        Renderer(RenderingBackend backend, Window* window);
        ~Renderer();

        void Render(double deltaTime);
    private:
        CREATE_BACKEND_FUNCTIONS(void, Init)
        CREATE_BACKEND_FUNCTIONS(void, InitAfterDeviceCreation)
        CREATE_BACKEND_FUNCTIONS(void, BeginFrame)
        CREATE_BACKEND_FUNCTIONS(void, PresentFrame)
        CREATE_BACKEND_FUNCTIONS(void, CleanupPreDevice)
        CREATE_BACKEND_FUNCTIONS(void, Cleanup)

        [[nodiscard]] std::vector<char> LoadShaderCode(const std::string &shaderFile, const ShaderType shaderType) const {
            std::string extension = m_Backend == RenderingBackend::Vulkan ? ".spv" : ".dxil";

            std::string stageName;
            switch (shaderType) {
                case ShaderType::Vertex: stageName = ".vertex"; break;
                case ShaderType::Fragment: stageName = ".fragment"; break;
            }

            std::filesystem::path path = g_ShadersDirectory + shaderFile + stageName + extension;

            if (!std::filesystem::exists(path)) {
                throw std::runtime_error("Shader missing: " + shaderFile + stageName + extension);
            }

            return readFile(path);
        }

        void GenerateFramebuffers();
    private:
        RenderingBackend m_Backend;
        RendererData* m_RendererData;

        Device* m_Device;
        Window* m_Window;

        std::vector<SwapChainImage> m_SwapChainImages;
        uint32_t m_SwapChainIndex = static_cast<uint32_t>(-1);

        std::vector<nvrhi::FramebufferHandle> m_Framebuffers;

        std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
        std::vector<nvrhi::EventQueryHandle> m_QueryPool;

        std::vector<nvrhi::GraphicsPipelineHandle> m_GraphicsPipelines;
        std::vector<nvrhi::GraphicsState> m_GraphicsStates;

        nvrhi::BindingSetHandle m_BindingSet;
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::InputLayoutHandle m_InputLayout;

        // Shaders
        nvrhi::ShaderHandle m_VertexShader;
        nvrhi::ShaderHandle m_PixelShader;

        // Buffers
        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
    };

}

#endif //NITRONIC_RENDERER_H