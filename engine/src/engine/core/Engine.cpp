//
// Created by simon on 28/08/2025.
//


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine/Engine.h"

#include <chrono>
#include <iostream>
#include <numeric>

NAMESPACE {

    Engine::Engine(const int windowWidth, const int windowHeight, const char* windowTitle, RenderingBackend backend)
        : m_TotalTimePassed(0), m_FPSCalcTimePassed(0) {
        m_Window = new Window(windowWidth, windowHeight, windowTitle);
        m_Renderer = new Renderer(backend, m_Window);
    }

    Engine::~Engine() {
        delete m_Renderer;
        delete m_Window;
    }

    void Engine::Run() {
        using clock = std::chrono::high_resolution_clock;
        using time_point = std::chrono::time_point<clock>;

        time_point previousTime = clock::now();

        while (!m_Window->ShouldClose()) {
            time_point currentTime = clock::now();
            std::chrono::duration<double> elapsed = currentTime - previousTime;
            const double deltaTime = elapsed.count(); // seconds
            m_TotalTimePassed += deltaTime;
            m_FPSCalcTimePassed += deltaTime;
            m_DeltaTimes.push_back(deltaTime);
            previousTime = currentTime;
            //std::cout << "DeltaTime: " << deltaTime << "s (" << 1.0f / deltaTime << "FPS)" << std::endl;

            if (m_FPSCalcTimePassed >= 0.1 && !m_DeltaTimes.empty()) {
                m_FPSCalcTimePassed = 0;
                const double mean = std::accumulate(m_DeltaTimes.begin(), m_DeltaTimes.end(), 0.0) / m_DeltaTimes.size();
                m_DeltaTimes.clear();

                std::stringstream ss;
                ss << "Nitronic Engine - " << 1.0f / mean << "FPS (" << mean << "ms)";
                m_Window->SetTitle(ss.str().c_str());
            }

            Window::PollEvents();

            m_Renderer->Render(deltaTime);

            m_Window->SwapBuffers();
        }
    }

}