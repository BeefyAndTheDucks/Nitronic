//
// Created by simon on 28/08/2025.
//

#include <stdexcept>

#include "core/Macros.h"
#include "engine/Window.h"

#include <format>

#include "stb_image.h"

NAMESPACE {

    constexpr int resolutions[8] = {
        16, 24, 32, 48, 64, 128, 256, 512
    };

    Window::Window(const int width, const int height, const char* title)
        : m_Width(width), m_Height(height) {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        GLFWimage icons[std::size(resolutions)];
        for (int i = 0; i < std::size(resolutions); i++) {
            WindowIcon icon{};
            icon.pixels = stbi_load(std::format("../assets/logo_{}.png", resolutions[i]).c_str(), &icon.width, &icon.height, nullptr, 4);
            m_Icons.push_back(icon);

            icons[i].width = icon.width;
            icons[i].height = icon.height;
            icons[i].pixels = icon.pixels;
        }
        glfwSetWindowIcon(m_Window, 1, icons);
    }

    Window::~Window() {
        for (const WindowIcon& icon : m_Icons) {
            stbi_image_free(icon.pixels);
        }

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::SwapBuffers() const {
        glfwSwapBuffers(m_Window);
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
}
