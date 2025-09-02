//
// Created by simon on 28/08/2025.
//

#include "Window.h"

#include <stdexcept>

namespace Nitronic {

    Window::Window(const int width, const int height, const char *title) {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

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
