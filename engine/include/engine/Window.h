//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_WINDOW_H
#define NITRONIC_WINDOW_H
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "core/Macros.h"

NAMESPACE {

    class Window {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        static void PollEvents();
        [[nodiscard]] bool ShouldClose() const;

        [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }
    private:
        GLFWwindow* m_Window;
    };

}

#endif //NITRONIC_WINDOW_H