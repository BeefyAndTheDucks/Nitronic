//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_WINDOW_H
#define NITRONIC_WINDOW_H
#include <vector>

#include "GlfwInclude.h"

#include "core/Macros.h"

NAMESPACE {

    struct WindowIcon {
        unsigned char* pixels;
        int width;
        int height;
    };

    class Window {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        static void PollEvents();

        void SwapBuffers() const;

        [[nodiscard]] bool ShouldClose() const;

        [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }

        void GetFramebufferSize(int* width, int* height) const;
        [[nodiscard]] int GetFramebufferWidth() const;
        [[nodiscard]] int GetFramebufferHeight() const;

        [[nodiscard]] std::vector<WindowIcon> GetIcons() const { return m_Icons; }

        [[nodiscard]] bool IsMinimized() const;

        void SetTitle(const char* title) const;

        void Close() const { glfwSetWindowShouldClose(m_Window, true); }
        void CancelClose() const { glfwSetWindowShouldClose(m_Window, false); }
    private:
        int m_Width;
        int m_Height;

        GLFWwindow* m_Window;

        std::vector<WindowIcon> m_Icons;
    };

}

#endif //NITRONIC_WINDOW_H