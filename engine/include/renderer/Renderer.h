//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include <filesystem>
#include <queue>

#include "core/Enums.h"
#include "core/Macros.h"

#include "engine/Window.h"
#include "engine/Log.h"

#include "Camera.h"
#include "Device.h"
#include "ImGuiRenderer.h"
#include "OffscreenFramebuffer.h"
#include "PSOCache.h"
#include "RendererComponents.h"
#include "engine/Event.h"

NAMESPACE {

    struct SwapChainImage {
        nvrhi::TextureHandle nvrhiHandle;
    };

    struct alignas(16) ModelConstants {
        glm::mat4 viewProj;
        glm::mat4 model;
        glm::mat4 normalMatrix;
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
        Renderer(RenderingBackend backend, Window* window, EventBus& eventBus);
        ~Renderer();

        void BeginScene(const Camera& camera);

        void RenderRenderables(entt::registry &scene) const;

        void EndScene();

        /// Creates a new offscreen framebuffer. Caller owns the returned pointer.
        [[nodiscard]] std::unique_ptr<OffscreenFramebuffer> CreateOffscreenFramebuffer(const OffscreenFramebufferDesc& desc) const;

        /// Resizes an existing offscreen framebuffer. Handles GPU sync internally.
        void ResizeOffscreenFramebuffer(OffscreenFramebuffer& fb, uint32_t width, uint32_t height) const;

        /// Destroys the GPU resources held by the framebuffer. Call before destroying.
        void DestroyOffscreenFramebuffer(OffscreenFramebuffer& fb) const;

        /// Sets the render target for 3D model rendering.
        /// Pass nullptr to render to the SwapChain backbuffer instead of a custom offscreen FrameBuffer.
        void Set3DRenderTarget(OffscreenFramebuffer* target);

        /// Queues a resize, which will be applied at the start of the next frame.
        void RequestOffscreenResize(OffscreenFramebuffer& fb, uint32_t width, uint32_t height);

        [[nodiscard]] RendererData* GetRendererData() const { return m_RendererData.get(); }
        [[nodiscard]] Device* GetDevice() const { return m_Device.get(); }
        [[nodiscard]] uint32_t GetSwapChainIndex() const { return m_SwapChainIndex; }

    private:
        CREATE_BACKEND_FUNCTIONS(void, Init)
        CREATE_BACKEND_FUNCTIONS(void, InitAfterDeviceCreation)
        CREATE_BACKEND_FUNCTIONS(void, BeginFrame)
        CREATE_BACKEND_FUNCTIONS(void, PresentFrame)
        CREATE_BACKEND_FUNCTIONS(void, CleanupPreDevice)
        CREATE_BACKEND_FUNCTIONS(void, Cleanup)

        void GenerateBackbuffers();
        void BuildOffscreenFramebufferImages(OffscreenFramebuffer& fb, uint32_t width, uint32_t height) const;
        void FlushPendingResizes();

        void CheckInitializeRendered(Rendered &rendered, const nvrhi::FramebufferHandle &targetFB) const;

        void OnFramebufferResized(FramebufferResizeEvent e);

    private:
        RenderingBackend m_Backend;
        Window* m_Window; // Borrowed from Engine
        EventBus& m_EventBus;

        // Data
        std::unique_ptr<RendererData> m_RendererData;
        std::unique_ptr<Device> m_Device;

        // Swapchain
        std::vector<SwapChainImage> m_SwapChainImages;
        uint32_t m_SwapChainIndex = static_cast<uint32_t>(-1);

        // Framebuffers
        std::vector<nvrhi::FramebufferHandle> m_Backbuffers;
        std::vector<nvrhi::TextureHandle> m_BackbufferDepthStencilTextures;
        bool m_ForceResizeSwapchain = false;

        // 3D render target, nullptr = swapchain
        OffscreenFramebuffer* m_3DRenderTarget = nullptr;

        // Pending resizes (applied between frames)
        struct PendingResize {
            OffscreenFramebuffer* fb;
            uint32_t width;
            uint32_t height;
        };
        std::vector<PendingResize> m_PendingResizes;

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

        // Textures
        nvrhi::SamplerHandle m_Sampler;

        // Command Lists
        nvrhi::CommandListHandle m_RenderingCommandList;

        // Matrices
        glm::mat4 m_ViewProjectionMatrix;
    };

}

#endif //NITRONIC_RENDERER_H