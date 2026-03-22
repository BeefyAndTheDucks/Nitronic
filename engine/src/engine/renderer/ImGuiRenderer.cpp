//
// Created by simon on 18/01/2026.
//

#include "renderer/ImGuiRenderer.h"
#include "imgui_impl_glfw.h"

NAMESPACE {

    ImGuiRenderer::ImGuiRenderer(const RenderingBackend backend, Window* window, RendererData* rendererData, DeviceData* deviceData)
        : m_ImGuiRendererData(nullptr), m_RendererData(rendererData), m_DeviceData(deviceData), m_Window(window), m_Backend(backend)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        CREATE_BACKEND_SWITCH(Init);

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

        static auto original = platformIO.Platform_CreateWindow;
        static auto windowIcons = window->GetIcons();

        platformIO.Platform_CreateWindow = [](ImGuiViewport* vp) {
            if (!original)
                return;

            original(vp);

            std::vector<GLFWimage> icons;
            for (const auto &[pixels, width, height] : windowIcons) {
                icons.push_back({ width, height, pixels });
            }

            glfwSetWindowIcon(static_cast<GLFWwindow*>(vp->PlatformHandle), static_cast<int>(icons.size()), icons.data());
        };
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
