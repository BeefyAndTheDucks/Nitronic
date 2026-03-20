//
// Created by simon on 18/01/2026.
//

#include "renderer/ImGuiRenderer.h"

NAMESPACE {

    ImGuiRenderer::ImGuiRenderer(const RenderingBackend backend, Window* window, RendererData* rendererData, DeviceData* deviceData, const nvrhi::FramebufferHandle &framebuffer)
        : m_ImGuiRendererData(nullptr), m_RendererData(rendererData), m_DeviceData(deviceData), m_Window(window), m_Backend(backend)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        CREATE_BACKEND_SWITCH(Init, framebuffer);
    }

    ImGuiRenderer::~ImGuiRenderer() {
        CREATE_BACKEND_SWITCH(Shutdown);
        ImGui::DestroyContext();
    }

    void ImGuiRenderer::BeginFrame() const {
        CREATE_BACKEND_SWITCH(BeginFrame);

        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();
    }

    void ImGuiRenderer::Render(const nvrhi::CommandListHandle &commandList) const {
        ImGui::Render();

        CREATE_BACKEND_SWITCH(Render, commandList);

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ImGuiTexture ImGuiRenderer::AddTexture(const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler) const {
        return CREATE_BACKEND_SWITCH(AddTexture, texture, sampler);
    }

    void ImGuiRenderer::RemoveTexture(const ImGuiTexture &texture) const {
        return CREATE_BACKEND_SWITCH(RemoveTexture, texture);
    }
}
