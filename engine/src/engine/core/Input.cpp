//
// Created by simon on 28/03/2026.
//

#include "engine/Input.h"

#include "engine/Log.h"

NAMESPACE
{

    Input::Input(EventBus& eventBus)
        : m_EventBus(eventBus)
    {
        ENGINE_ASSERT(!s_Instance, "Input already exists!");
        s_Instance = this;

        m_EventBus.subscribe<KeyEvent,         &Input::OnKey>(*this);
        m_EventBus.subscribe<MouseButtonEvent, &Input::OnMouseButton>(*this);
        m_EventBus.subscribe<MouseMoveEvent,   &Input::OnMouseMove>(*this);
        m_EventBus.subscribe<MouseScrollEvent, &Input::OnMouseScroll>(*this);
    }

    Input::~Input()
    {
        m_EventBus.unsubscribe<KeyEvent,         &Input::OnKey>(*this);
        m_EventBus.unsubscribe<MouseButtonEvent, &Input::OnMouseButton>(*this);
        m_EventBus.unsubscribe<MouseMoveEvent,   &Input::OnMouseMove>(*this);
        m_EventBus.unsubscribe<MouseScrollEvent, &Input::OnMouseScroll>(*this);

        if (s_Instance == this)
            s_Instance = nullptr;
    }

    void Input::OnKey(const KeyEvent& e)
    {
        if (e.action == GLFW_PRESS)
        {
            m_CurrentKeys[e.key]      = true;
            m_PressedThisFrame[e.key] = true;
        } else if (e.action == GLFW_RELEASE)
        {
            m_CurrentKeys[e.key]       = false;
            m_ReleasedThisFrame[e.key] = true;
        }
    }

    void Input::OnMouseButton(const MouseButtonEvent& e)
    {
        m_MouseButtons[e.button] = e.action == GLFW_PRESS;
    }

    void Input::OnMouseMove(const MouseMoveEvent& e)
    {
        if (m_FirstMouse)
        {
            m_LastMouseX = e.x;
            m_LastMouseY = e.y;
            m_FirstMouse = false;
        }

        m_MouseX = e.x;
        m_MouseY = e.y;
    }

    void Input::OnMouseScroll(const MouseScrollEvent& e)
    {
        m_MouseScrollDeltaX += e.xOffset;
        m_MouseScrollDeltaY += e.yOffset;
    }

    void Input::EndFrame()
    {
        m_PressedThisFrame.clear();
        m_ReleasedThisFrame.clear();

        m_MouseDeltaX = m_MouseX - m_LastMouseX;
        m_MouseDeltaY = m_MouseY - m_LastMouseY;
        m_LastMouseX  = m_MouseX;
        m_LastMouseY  = m_MouseY;

        m_MouseScrollDeltaX = 0.0;
        m_MouseScrollDeltaY = 0.0;
    }

    bool Input::IsKeyDown(Key key) const { return m_CurrentKeys.contains(static_cast<int>(key)) && m_CurrentKeys.at(static_cast<int>(key)); }
    bool Input::IsKeyPressed(Key key) const { return m_PressedThisFrame.contains(static_cast<int>(key)) && m_PressedThisFrame.at(static_cast<int>(key)); }
    bool Input::IsKeyReleased(Key key) const { return m_ReleasedThisFrame.contains(static_cast<int>(key)) && m_ReleasedThisFrame.at(static_cast<int>(key)); }
    bool Input::IsMouseButtonDown(MouseButton b) const { return m_MouseButtons.contains(static_cast<int>(b)) && m_MouseButtons.at(static_cast<int>(b)); }
    void Input::GetMousePosition(double& x, double& y) const { x = m_MouseX; y = m_MouseY; }
    void Input::GetMouseDelta(double& x, double& y) const { x = m_MouseDeltaX; y = m_MouseDeltaY; }
    void Input::GetMouseScrollDelta(double& x, double& y) const { x = m_MouseScrollDeltaX; y = m_MouseScrollDeltaY; }

    bool Input::KeyDown(const Key key) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); return s_Instance->IsKeyDown(key); }
    bool Input::KeyPressed(const Key key) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); return s_Instance->IsKeyPressed(key); }
    bool Input::KeyReleased(const Key key) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); return s_Instance->IsKeyReleased(key); }
    bool Input::MouseButtonDown(const MouseButton b) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); return s_Instance->IsMouseButtonDown(b); }
    void Input::MousePosition(double& x, double& y) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); s_Instance->GetMousePosition(x, y); }
    void Input::MouseDelta(double& x, double& y) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); s_Instance->GetMouseDelta(x, y); }
    void Input::MouseScrollDelta(double& x, double& y) { ENGINE_ASSERT(s_Instance, "Input not initialized!"); s_Instance->GetMouseScrollDelta(x, y); }
}
