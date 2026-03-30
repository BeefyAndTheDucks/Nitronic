//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_INPUT_H
#define NITRONIC_INPUT_H

#include "core/Macros.h"
#include "GlfwInclude.h"

#include "Event.h"
#include "EventBus.h"

NAMESPACE
{

    enum class Key
    {
        Unknown = GLFW_KEY_UNKNOWN,

        Space = GLFW_KEY_SPACE,
        Apostrophe = GLFW_KEY_APOSTROPHE,
        Comma = GLFW_KEY_COMMA,
        Minus = GLFW_KEY_MINUS,
        Period = GLFW_KEY_PERIOD,
        Slash = GLFW_KEY_SLASH,
        Zero = GLFW_KEY_0,
        One = GLFW_KEY_1,
        Two = GLFW_KEY_2,
        Three = GLFW_KEY_3,
        Four = GLFW_KEY_4,
        Five = GLFW_KEY_5,
        Six = GLFW_KEY_6,
        Seven = GLFW_KEY_7,
        Eight = GLFW_KEY_8,
        Nine = GLFW_KEY_9,
        Semicolon = GLFW_KEY_SEMICOLON,
        Equal = GLFW_KEY_EQUAL,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        LeftBracket = GLFW_KEY_LEFT_BRACKET,
        Backslash = GLFW_KEY_BACKSLASH,
        RightBracket = GLFW_KEY_RIGHT_BRACKET,
        GraveAccent = GLFW_KEY_GRAVE_ACCENT,
        World1 = GLFW_KEY_WORLD_1,
        World2 = GLFW_KEY_WORLD_2,

        Escape = GLFW_KEY_ESCAPE,
        Enter = GLFW_KEY_ENTER,
        Tab = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
        Insert = GLFW_KEY_INSERT,
        Delete = GLFW_KEY_DELETE,
        Right = GLFW_KEY_RIGHT,
        Left = GLFW_KEY_LEFT,
        Down = GLFW_KEY_DOWN,
        Up = GLFW_KEY_UP,
        PageUp = GLFW_KEY_PAGE_UP,
        PageDown = GLFW_KEY_PAGE_DOWN,
        Home = GLFW_KEY_HOME,
        End = GLFW_KEY_END,
        CapsLock = GLFW_KEY_CAPS_LOCK,
        ScrollLock = GLFW_KEY_SCROLL_LOCK,
        NumLock = GLFW_KEY_NUM_LOCK,
        PrintScreen = GLFW_KEY_PRINT_SCREEN,
        Pause = GLFW_KEY_PAUSE,
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,
        F13 = GLFW_KEY_F13,
        F14 = GLFW_KEY_F14,
        F15 = GLFW_KEY_F15,
        F16 = GLFW_KEY_F16,
        F17 = GLFW_KEY_F17,
        F18 = GLFW_KEY_F18,
        F19 = GLFW_KEY_F19,
        F20 = GLFW_KEY_F20,
        F21 = GLFW_KEY_F21,
        F22 = GLFW_KEY_F22,
        F23 = GLFW_KEY_F23,
        F24 = GLFW_KEY_F24,
        F25 = GLFW_KEY_F25,
        KeyPad0 = GLFW_KEY_KP_0,
        KeyPad1 = GLFW_KEY_KP_1,
        KeyPad2 = GLFW_KEY_KP_2,
        KeyPad3 = GLFW_KEY_KP_3,
        KeyPad4 = GLFW_KEY_KP_4,
        KeyPad5 = GLFW_KEY_KP_5,
        KeyPad6 = GLFW_KEY_KP_6,
        KeyPad7 = GLFW_KEY_KP_7,
        KeyPad8 = GLFW_KEY_KP_8,
        KeyPad9 = GLFW_KEY_KP_9,
        KeyPadDecimal = GLFW_KEY_KP_DECIMAL,
        KeyPadDivide = GLFW_KEY_KP_DIVIDE,
        KeyPadMultiply = GLFW_KEY_KP_MULTIPLY,
        KeyPadSubtract = GLFW_KEY_KP_SUBTRACT,
        KeyPadAdd = GLFW_KEY_KP_ADD,
        KeyPadEnter = GLFW_KEY_KP_ENTER,
        KeyPadEqual = GLFW_KEY_KP_EQUAL,
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        LeftControl = GLFW_KEY_LEFT_CONTROL,
        LeftAlt = GLFW_KEY_LEFT_ALT,
        LeftSuper = GLFW_KEY_LEFT_SUPER,
        RightShift = GLFW_KEY_RIGHT_SHIFT,
        RightControl = GLFW_KEY_RIGHT_CONTROL,
        RightAlt = GLFW_KEY_RIGHT_ALT,
        RightSuper = GLFW_KEY_RIGHT_SUPER,
        Menu = GLFW_KEY_MENU,

        LeftCtrl = LeftControl,
        RightCtrl = RightControl,
    };

    enum class MouseButton
    {
        Button1 = GLFW_MOUSE_BUTTON_1,
        Button2 = GLFW_MOUSE_BUTTON_2,
        Button3 = GLFW_MOUSE_BUTTON_3,
        Button4 = GLFW_MOUSE_BUTTON_4,
        Button5 = GLFW_MOUSE_BUTTON_5,
        Button6 = GLFW_MOUSE_BUTTON_6,
        Button7 = GLFW_MOUSE_BUTTON_7,
        Button8 = GLFW_MOUSE_BUTTON_8,
        ButtonLast = GLFW_MOUSE_BUTTON_LAST,
        ButtonLeft = GLFW_MOUSE_BUTTON_LEFT,
        ButtonRight = GLFW_MOUSE_BUTTON_RIGHT,
        ButtonMiddle = GLFW_MOUSE_BUTTON_MIDDLE,

        Left = ButtonLeft,
        Right = ButtonRight,
        Middle = ButtonMiddle,
    };

    class Input
    {
    public:
        explicit Input(EventBus& eventBus);
        ~Input();

        [[nodiscard]] bool IsKeyDown        (Key key             ) const;
        [[nodiscard]] bool IsKeyPressed     (Key key             ) const;  // true only on the frame it was pressed
        [[nodiscard]] bool IsKeyReleased    (Key key             ) const;  // true only on the frame it was released
        [[nodiscard]] bool IsMouseButtonDown(MouseButton b       ) const;
        void GetMousePosition               (double& x, double& y) const;
        void GetMouseDelta                  (double& x, double& y) const;
        void GetMouseScrollDelta            (double& x, double& y) const;

        static bool KeyDown                 (Key key             );
        static bool KeyPressed              (Key key             );
        static bool KeyReleased             (Key key             );
        static bool MouseButtonDown         (MouseButton b       );
        static void MousePosition           (double& x, double& y);
        static void MouseDelta              (double& x, double& y);
        static void MouseScrollDelta        (double& x, double& y);

        void EndFrame();

    private:
        void OnKey(const KeyEvent& e);
        void OnMouseButton(const MouseButtonEvent& e);
        void OnMouseMove(const MouseMoveEvent& e);
        void OnMouseScroll(const MouseScrollEvent& e);

    private:
        std::unordered_map<int, bool> m_CurrentKeys;
        std::unordered_map<int, bool> m_PressedThisFrame;
        std::unordered_map<int, bool> m_ReleasedThisFrame;
        std::unordered_map<int, bool> m_MouseButtons;

        double m_MouseX      = 0.0;
        double m_MouseY      = 0.0;
        double m_LastMouseX  = 0.0;
        double m_LastMouseY  = 0.0;
        double m_MouseDeltaX = 0.0;
        double m_MouseDeltaY = 0.0;
        bool m_FirstMouse    = true; // Prevent big deltas on first check

        double m_MouseScrollDeltaX = 0.0;
        double m_MouseScrollDeltaY = 0.0;

        EventBus& m_EventBus;

        inline static Input* s_Instance = nullptr;
    };

}

#endif //NITRONIC_INPUT_H
