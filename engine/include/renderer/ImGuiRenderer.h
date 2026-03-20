//
// Created by simon on 18/01/2026.
//

#ifndef NITRONIC_IMGUIRENDERER_H
#define NITRONIC_IMGUIRENDERER_H
#include <imgui.h>
#include "core/Enums.h"
#include "core/Macros.h"
#include "engine/Window.h"
#include "renderer/RendererTypes.h"

NAMESPACE {

    struct ImGuiTexture {
        nvrhi::TextureHandle texture;
        nvrhi::SamplerHandle sampler;
        ImTextureID textureID;
    };

    class ImGuiRenderer {
    public:
        ImGuiRenderer(RenderingBackend backend, Window* window, RendererData* rendererData, DeviceData* deviceData, const nvrhi::FramebufferHandle& framebuffer);
        ~ImGuiRenderer();

        void BeginFrame() const;
        void Render(const nvrhi::CommandListHandle &commandList) const;

        [[nodiscard]] ImGuiTexture AddTexture(const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler) const;
        void RemoveTexture(const ImGuiTexture& texture) const;
    private:
        CREATE_BACKEND_FUNCTIONS(void, Init, const nvrhi::FramebufferHandle& framebuffer)
        CREATE_BACKEND_FUNCTIONS(void, Shutdown)

        CREATE_BACKEND_FUNCTIONS(static void, BeginFrame)
        CREATE_BACKEND_FUNCTIONS(static void, Render, const nvrhi::CommandListHandle &commandList)

        CREATE_BACKEND_FUNCTIONS([[nodiscard]] static ImGuiTexture, AddTexture, const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler)
        CREATE_BACKEND_FUNCTIONS(static void, RemoveTexture, const ImGuiTexture& texture)
    private:
        ImGuiRendererData* m_ImGuiRendererData;

        RendererData* m_RendererData;
        DeviceData* m_DeviceData;

        Window* m_Window;
        RenderingBackend m_Backend;
    };

}

#endif //NITRONIC_IMGUIRENDERER_H