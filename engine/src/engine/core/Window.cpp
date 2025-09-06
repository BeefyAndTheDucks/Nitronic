//
// Created by simon on 28/08/2025.
//

#include <stdexcept>

#include "core/Macros.h"
#include "engine/Window.h"

NAMESPACE {

    Window::Window(const int width, const int height, const char *title) {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
    }

    Window::~Window() {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }



}
