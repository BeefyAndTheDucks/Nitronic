//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include <filesystem>
#include <iostream>
#include <queue>

#include "Camera.h"
#include "core/Enums.h"
#include "core/Macros.h"

#include "Device.h"
#include "ImGuiRenderer.h"
#include "Model.h"
#include "PSOCache.h"
#include "engine/Window.h"

NAMESPACE {

    struct SwapChainImage {
        //vk::Image image;
        nvrhi::TextureHandle nvrhiHandle;
    };

    struct ImGuiVertex {
        float position[2];
        float uv[2];
        float color[3];
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

        void BeginScene(const Camera& camera);

        void RenderModel(Model* model);

        void EndScene();

        void RegenerateFrambuffers(float requestedWidth, float requestedHeight);

        [[nodiscard]] RendererData* GetRendererData() const { return m_RendererData; }
        [[nodiscard]] Device* GetDevice() const { return m_Device; }

    private:
        CREATE_BACKEND_FUNCTIONS(void, Init)
        CREATE_BACKEND_FUNCTIONS(void, InitAfterDeviceCreation)
        CREATE_BACKEND_FUNCTIONS(void, BeginFrame)
        CREATE_BACKEND_FUNCTIONS(void, PresentFrame)
        CREATE_BACKEND_FUNCTIONS(void, CleanupPreDevice)
        CREATE_BACKEND_FUNCTIONS(void, Cleanup)

        void GenerateBackbuffers();

    private:
        RenderingBackend m_Backend;
        RendererData* m_RendererData;

        Device* m_Device;
        Window* m_Window;

        std::vector<SwapChainImage> m_SwapChainImages;
        uint32_t m_SwapChainIndex = static_cast<uint32_t>(-1);

        std::vector<nvrhi::FramebufferHandle> m_Backbuffers;

        std::vector<nvrhi::FramebufferHandle> m_Framebuffers;
        float m_FramebufferWidth = -1, m_FramebufferHeight = -1;

        std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
        std::vector<nvrhi::EventQueryHandle> m_QueryPool;

        bool m_HasImGuiGraphicsPipeline = false;
        nvrhi::GraphicsPipelineHandle m_ImGuiGraphicsPipeline;

        std::optional<ShaderCache> m_ShaderCache;
        std::optional<PSOCache> m_PSOCache;

        nvrhi::BindingSetHandle m_BindingSet;
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::InputLayoutHandle m_InputLayout;

        // Buffers
        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;

        // ImGui
        ImGuiRenderer* m_ImGuiRenderer;
        std::vector<ImGuiTexture> m_ImGuiFramebufferTextures;
        bool m_HasGeneratedImGuiFramebuffer = false;
        bool m_ValidGameWindow = false;

        // Textures
        nvrhi::SamplerHandle m_Sampler;

        // Command Lists
        nvrhi::CommandListHandle m_RenderingCommandList;

        // Matrices
        glm::mat4 m_ViewProjectionMatrix;
    };

}

#endif //NITRONIC_RENDERER_H