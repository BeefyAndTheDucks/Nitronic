//
// Created by simon on 18/01/2026.
//

#include "renderer/ImGuiRenderer.hpp"
#include "imgui_impl_glfw.h"
#include "engine/Event.hpp"

NAMESPACE
{

    ImGuiRenderer* ImGuiRenderer::s_Instance = nullptr;

    ImGuiRenderer::ImGuiRenderer(const RenderingBackend backend, Window* window, RendererData* rendererData, DeviceData* deviceData, EventBus& eventBus)
        : m_ImGuiRendererData(nullptr), m_RendererData(rendererData), m_DeviceData(deviceData), m_Window(window),
          m_Backend(backend), m_EventBus(eventBus)
    {
        s_Instance = this;

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

        platformIO.Platform_CreateWindow = [](ImGuiViewport* vp)
        {
            if (!original)
                return;

            original(vp);

            const auto platformWindow = static_cast<GLFWwindow*>(vp->PlatformHandle);

            std::vector<GLFWimage> icons;
            for (const auto& [pixels, width, height] : windowIcons)
            {
                icons.push_back({width, height, pixels});
            }

            s_Instance->m_OriginalKeyCallback = glfwSetKeyCallback(platformWindow, [](GLFWwindow* eventWindow, const int key, const int scancode, const int action, const int mods)
            {
                s_Instance->m_OriginalKeyCallback(eventWindow, key, scancode, action, mods);
                s_Instance->m_EventBus.dispatch(KeyEvent{key, scancode, action, mods});
            });

            s_Instance->m_OriginalMouseButtonCallback = glfwSetMouseButtonCallback(platformWindow, [](GLFWwindow* eventWindow, const int button, const int action, const int mods)
            {
                s_Instance->m_OriginalMouseButtonCallback(eventWindow, button, action, mods);
                s_Instance->m_EventBus.dispatch(MouseButtonEvent{button, action, mods});
            });

            s_Instance->m_OriginalCursorPosCallback = glfwSetCursorPosCallback(platformWindow, [](GLFWwindow* eventWindow, const double xPos, const double yPos)
            {
                s_Instance->m_OriginalCursorPosCallback(eventWindow, xPos, yPos);
                s_Instance->m_EventBus.dispatch(MouseMoveEvent{xPos, yPos});
            });

            s_Instance->m_OriginalScrollCallback = glfwSetScrollCallback(platformWindow, [](GLFWwindow* eventWindow, const double xOffset, const double yOffset)
            {
                s_Instance->m_OriginalScrollCallback(eventWindow, xOffset, yOffset);
                s_Instance->m_EventBus.dispatch(MouseScrollEvent{xOffset, yOffset});
            });

            s_Instance->m_OriginalCharCallback = glfwSetCharCallback(platformWindow, [](GLFWwindow* eventWindow, const unsigned int codepoint)
            {
                s_Instance->m_OriginalCharCallback(eventWindow, codepoint);
                s_Instance->m_EventBus.dispatch(CharInputEvent{codepoint});
            });

            glfwSetWindowIcon(platformWindow, static_cast<int>(icons.size()), icons.data());
        };
    }

    ImGuiRenderer::~ImGuiRenderer() {
        CREATE_BACKEND_SWITCH(Shutdown);
        ImGui::DestroyContext();
    }

    void ImGuiRenderer::BeginFrame() const {
        CREATE_BACKEND_SWITCH(BeginFrame);

        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
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
