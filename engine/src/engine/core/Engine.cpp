//
// Created by simon on 28/08/2025.
//

#include "engine/Engine.h"

namespace Nitronic {

    Engine::Engine() {
        m_Window = new Window(1280, 720, "Nitronic");
    }

    Engine::~Engine() {
        delete m_Window;
    }

    void Engine::Run() {
        while (!m_Window->ShouldClose()) {
            Window::PollEvents();
        }
    }


}