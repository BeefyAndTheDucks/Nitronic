//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include <iostream>
#include <queue>

#include "core/Enums.h"
#include "core/Macros.h"

#include "Device.h"
#include "engine/Window.h"

NAMESPACE {

    constexpr size_t g_MaxFramesInFlight = 2;

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
        CREATE_BACKEND_FUNCTIONS(Init)
        CREATE_BACKEND_FUNCTIONS(InitAfterDeviceCreation)
        CREATE_BACKEND_FUNCTIONS(BeginFrame)
        CREATE_BACKEND_FUNCTIONS(PresentFrame)
        CREATE_BACKEND_FUNCTIONS(CleanupPreDevice)
        CREATE_BACKEND_FUNCTIONS(Cleanup)

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