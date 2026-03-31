//
// Created by simon on 28/08/2025.
//

#include <stdexcept>

#include "core/Macros.h"
#include "engine/Window.h"

#include <format>
#include <stb_image.h>
#include <tracy/Tracy.hpp>

#include "core/Constants.h"
#include "engine/Event.h"
#include "engine/Log.h"

NAMESPACE
{

    constexpr int g_IconResolutions[8] = {
        16, 24, 32, 48, 64, 128, 256, 512
    };

    Window::Window(const int width, const int height, const char* title, EventBus& eventBus)
        : m_Width(width), m_Height(height), m_EventBus(eventBus) {
        glfwSetErrorCallback(OnGLFWError);

        ENGINE_ASSERT(glfwInit(), "Failed to initialize GLFW");

        ENGINE_ASSERT(glfwVulkanSupported(), "GLFW doesn't support Vulkan in this environment.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            ENGINE_ABORT("Failed to create GLFW window");
        }
        glfwSetWindowUserPointer(m_Window, this);

        glfwSetWindowSizeCallback(m_Window,      OnWindowResized);
        glfwSetWindowCloseCallback(m_Window,     OnWindowClose);
        glfwSetWindowFocusCallback(m_Window,     OnWindowFocus);
        glfwSetKeyCallback(m_Window,             OnKey);
        glfwSetMouseButtonCallback(m_Window,     OnMouseButton);
        glfwSetCursorPosCallback(m_Window,       OnCursorPos);
        glfwSetScrollCallback(m_Window,          OnScroll);
        glfwSetCharCallback(m_Window,            OnChar);
        glfwSetFramebufferSizeCallback(m_Window, OnFramebufferResize);

        if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND && glfwGetPlatform() != GLFW_PLATFORM_COCOA) { // Wayland/MacOS don't support setting window icons
            GLFWimage icons[std::size(g_IconResolutions)];
            for (int i = 0; i < std::size(g_IconResolutions); i++) {
                WindowIcon icon{};
                icon.pixels = stbi_load(std::format("{}logo_{}.png", g_AssetsDirectory, g_IconResolutions[i]).c_str(), &icon.width, &icon.height, nullptr, 4);
                m_Icons.push_back(icon);

                icons[i].width = icon.width;
                icons[i].height = icon.height;
                icons[i].pixels = icon.pixels;
            }
            glfwSetWindowIcon(m_Window, std::size(g_IconResolutions), icons);
        }
    }

    Window::~Window() {
        for (const WindowIcon& icon : m_Icons) {
            stbi_image_free(icon.pixels);
        }

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::OnGLFWError(int errorCode, const char* description)
    {
        ENGINE_ERROR("GLFW Error: {} (Code {})", description, errorCode);
    }

    void Window::PollEvents() const {
        ZoneScoped;

        if (IsMinimized())
            glfwWaitEvents();
        else
            glfwPollEvents();
    }

    void Window::OnWindowResized(GLFWwindow* w, const int width, const int height)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(WindowResizeEvent{width, height});
    }

    void Window::OnWindowClose(GLFWwindow* w)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(WindowCloseEvent{});
    }

    void Window::OnWindowFocus(GLFWwindow* w, const int focused)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(WindowFocusEvent{ focused == GLFW_TRUE });
    }

    void Window::OnKey(GLFWwindow* w, const int key, const int scancode, const int action, const int mods)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(KeyEvent{ key, scancode, action, mods });
    }

    void Window::OnMouseButton(GLFWwindow* w, const int button, const int action, const int mods)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(MouseButtonEvent{ button, action, mods });
    }

    void Window::OnCursorPos(GLFWwindow* w, const double x, const double y)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(MouseMoveEvent{ x, y });
    }

    void Window::OnScroll(GLFWwindow* w, const double xOffset, const double yOffset)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(MouseScrollEvent{ xOffset, yOffset });
    }

    void Window::OnChar(GLFWwindow* w, const uint32_t codepoint)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(CharInputEvent{ codepoint });
    }

    void Window::OnFramebufferResize(GLFWwindow* w, const int width, const int height)
    {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->m_EventBus.dispatch(FramebufferResizeEvent{ width, height });
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::GetFramebufferSize(int *width, int *height) const {
        glfwGetFramebufferSize(m_Window, width, height);
    }

    int Window::GetFramebufferWidth() const {
        int width, height;
        GetFramebufferSize(&width, &height);
        return width;
    }

    int Window::GetFramebufferHeight() const {
        int width, height;
        GetFramebufferSize(&width, &height);
        return height;
    }

    bool Window::IsMinimized() const {
        const bool isMinimized = glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
        return isMinimized;
    }

    void Window::SetTitle(const char *title) const {
        glfwSetWindowTitle(m_Window, title);
    }

    void Window::SetCursorPosition(const double xPos, const double yPos) const {
        if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
            ENGINE_WARN("GLFW doesn't support setting the cursors position on Wayland.");
            return;
        }

        glfwSetCursorPos(m_Window, xPos, yPos);
    }

    void Window::SetCursorMode(const CursorMode& cursorMode) const {
        switch (cursorMode) {
            case CursorMode::Normal:
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case CursorMode::Hidden:
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            case CursorMode::Disabled:
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case CursorMode::Captured:
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                break;
        }
    }
}
