//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_WINDOW_H
#define NITRONIC_WINDOW_H
#include <vector>

#include "EventBus.h"
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
        Window(int width, int height, const char* title, EventBus& eventBus);
        ~Window();

        static void OnGLFWError(int errorCode, const char* description);

        void PollEvents() const;

        static void OnWindowResized(GLFWwindow* w, int width, int height);
        static void OnWindowClose(GLFWwindow* w);
        static void OnWindowFocus(GLFWwindow* w, int focused);
        static void OnKey(GLFWwindow* w, int key, int scancode, int action, int mods);
        static void OnMouseButton(GLFWwindow* w, int button, int action, int mods);
        static void OnCursorPos(GLFWwindow* w, double x, double y);
        static void OnScroll(GLFWwindow* w, double xOffset, double yOffset);
        static void OnChar(GLFWwindow* w, unsigned int codepoint);
        static void OnFramebufferResize(GLFWwindow* w, int width, int height);

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

        EventBus& m_EventBus;
    };

}

#endif //NITRONIC_WINDOW_H