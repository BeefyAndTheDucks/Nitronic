//
// Created by simon on 28/08/2025.
//

#include "engine/Engine.h"

#include <chrono>
#include <iostream>

NAMESPACE {

    Engine::Engine(const int windowWidth, const int windowHeight, const char* windowTitle, RenderingBackend backend) {
        m_Window = new Window(windowWidth, windowHeight, windowTitle);
        m_Renderer = new Renderer(backend);
    }

    Engine::~Engine() {
        delete m_Window;
        delete m_Renderer;
    }

    void Engine::Run() {
        using clock = std::chrono::high_resolution_clock;
        using time_point = std::chrono::time_point<clock>;

        time_point previousTime = clock::now();

        while (!m_Window->ShouldClose()) {
            time_point currentTime = clock::now();
            std::chrono::duration<double> elapsed = currentTime - previousTime;
            double deltaTime = elapsed.count(); // seconds
            previousTime = currentTime;
            std::cout << "DeltaTime: " << deltaTime << "s (" << 1.0f / deltaTime << "FPS)" << std::endl;

            Window::PollEvents();

            m_Renderer->Render(deltaTime);
        }
    }

}