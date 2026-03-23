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

#include "engine/Window.h"
#include "engine/Log.h"

#include "Camera.h"
#include "Device.h"
#include "ImGuiRenderer.h"
#include "Model.h"
#include "PSOCache.h"

NAMESPACE
{

    struct SwapChainImage {
        nvrhi::TextureHandle nvrhiHandle;
    };

    struct NvrhiMessageCallback : nvrhi::IMessageCallback
    {
        static NvrhiMessageCallback& GetInstance()
        {
            static NvrhiMessageCallback s_Instance;
            return s_Instance;
        }

        void message(const nvrhi::MessageSeverity severity, const char* messageText) override {
            switch (severity)
            {
                case nvrhi::MessageSeverity::Info:
                    ENGINE_INFO("Nvrhi: {}", messageText);
                    break;
                case nvrhi::MessageSeverity::Warning:
                    ENGINE_WARN("Nvrhi: {}", messageText);
                    break;
                case nvrhi::MessageSeverity::Error:
                    ENGINE_ERROR("Nvrhi: {}", messageText);
                    break;
                case nvrhi::MessageSeverity::Fatal:
                    ENGINE_CRITICAL("Nvrhi: {}", messageText);
                    break;
            }
        }
    };

    class Renderer {
    public:
        Renderer(RenderingBackend backend, Window* window);
        ~Renderer();

        void BeginScene(const Camera& camera);

        void RenderModel(Model& model);

        void EndScene();

        void RegenerateFrambuffers(float requestedWidth, float requestedHeight);

        [[nodiscard]] RendererData* GetRendererData() const { return m_RendererData.get(); }
        [[nodiscard]] Device* GetDevice() const { return m_Device.get(); }

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
        Window* m_Window; // Borrowed from Engine

        // Data
        std::unique_ptr<RendererData> m_RendererData;
        std::unique_ptr<Device> m_Device;

        // Swapchain
        std::vector<SwapChainImage> m_SwapChainImages;
        uint32_t m_SwapChainIndex = static_cast<uint32_t>(-1);

        // Framebuffers
        std::vector<nvrhi::FramebufferHandle> m_Framebuffers;
        std::vector<nvrhi::FramebufferHandle> m_Backbuffers;
        float m_FramebufferWidth = -1, m_FramebufferHeight = -1;
        bool m_HasGeneratedImGuiFramebuffer = false;

        // Frames
        std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
        std::vector<nvrhi::EventQueryHandle> m_QueryPool;

        // Graphics Pipelines
        bool m_HasImGuiGraphicsPipeline = false;
        nvrhi::GraphicsPipelineHandle m_ImGuiGraphicsPipeline;

        // Caches
        std::unique_ptr<ShaderCache> m_ShaderCache;
        std::unique_ptr<PSOCache> m_PSOCache;

        // Bindings
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSet;

        // ImGui
        std::unique_ptr<ImGuiRenderer> m_ImGuiRenderer;

        std::vector<ImGuiTexture> m_ImGuiFramebufferColorTextures;
        std::vector<nvrhi::TextureHandle> m_ImGuiFramebufferDepthTextures;

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