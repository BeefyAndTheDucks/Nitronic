//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_EVENT_H
#define NITRONIC_EVENT_H

NAMESPACE {

    struct WindowResizeEvent      { int width, height; };
    struct WindowCloseEvent       {};
    struct WindowFocusEvent       { bool focused; };
    struct KeyEvent               { int key, scancode, action, mods; };
    struct MouseButtonEvent       { int button, action, mods; };
    struct MouseMoveEvent         { double x, y; };
    struct MouseScrollEvent       { double xOffset, yOffset; };
    struct CharInputEvent         { unsigned int codepoint; };
    struct FramebufferResizeEvent { int width, height; };

}

#endif //NITRONIC_EVENT_H