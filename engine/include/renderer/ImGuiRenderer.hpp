//
// Created by simon on 18/01/2026.
//

#ifndef NITRONIC_IMGUIRENDERER_HPP
#define NITRONIC_IMGUIRENDERER_HPP
#include <memory>

#include <nvrhi/nvrhi.h>
#include <imgui.h>

#include "core/Enums.hpp"
#include "core/Macros.hpp"

#include "engine/Window.hpp"

#include "renderer/RendererTypes.hpp"

NAMESPACE {

    struct ImGuiTexture {
        nvrhi::TextureHandle texture;
        nvrhi::SamplerHandle sampler;
        ImTextureID textureID;
    };

    class ImGuiRenderer {
    public:
        ImGuiRenderer(RenderingBackend backend, Window* window, RendererData* rendererData, DeviceData* deviceData, EventBus& eventBus);
        ~ImGuiRenderer();

        void BeginFrame() const;
        void Render(const nvrhi::CommandListHandle &commandList) const;

        [[nodiscard]] ImGuiTexture AddTexture(const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler) const;
        void RemoveTexture(const ImGuiTexture& texture) const;
    private:
        CREATE_BACKEND_FUNCTIONS(void, Init)
        CREATE_BACKEND_FUNCTIONS(void, Shutdown)

        CREATE_BACKEND_FUNCTIONS(static void, BeginFrame)
        CREATE_BACKEND_FUNCTIONS(static void, Render, const nvrhi::CommandListHandle &commandList)

        CREATE_BACKEND_FUNCTIONS([[nodiscard]] static ImGuiTexture, AddTexture, const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler)
        CREATE_BACKEND_FUNCTIONS(static void, RemoveTexture, const ImGuiTexture& texture)
    private:
        std::unique_ptr<ImGuiRendererData> m_ImGuiRendererData;

        RendererData* m_RendererData; // Borrowed from Renderer
        DeviceData* m_DeviceData; // Borrowed from Device

        Window* m_Window; // Borrowed from Engine
        RenderingBackend m_Backend;

        EventBus& m_EventBus;

        GLFWkeyfun m_OriginalKeyCallback;
        GLFWmousebuttonfun m_OriginalMouseButtonCallback;
        GLFWcursorposfun m_OriginalCursorPosCallback;
        GLFWscrollfun m_OriginalScrollCallback;
        GLFWcharfun m_OriginalCharCallback;

        static ImGuiRenderer* s_Instance;
    };

}

#endif //NITRONIC_IMGUIRENDERER_HPP