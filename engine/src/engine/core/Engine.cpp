//
// Created by simon on 28/08/2025.
//


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine/Engine.h"

#include <chrono>
#include <iostream>
#include <numeric>

#include "renderer/Shaders.h"

NAMESPACE {

    static const std::vector<Vertex> g_Vertices = {
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} }, // front face
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

        { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} }, // right side face
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} },

        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // left side face
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

        { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // back face
        { {-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },

        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f} }, // top face
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} },

        { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} }, // bottom face
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
    };

    static const std::vector<uint32_t> g_Indices = {
        0,  1,  2,   0,  3,  1, // front face
        4,  5,  6,   4,  7,  5, // left face
        8,  9, 10,   8, 11,  9, // right face
       12, 13, 14,  12, 15, 13, // back face
       16, 17, 18,  16, 19, 17, // top face
       20, 21, 22,  20, 23, 21, // bottom face
    };

    Engine::Engine(const int windowWidth, const int windowHeight, const char* windowTitle, RenderingBackend backend)
        : m_TotalTimePassed(0), m_FPSCalcTimePassed(0) {
        m_Window = new Window(windowWidth, windowHeight, windowTitle);
        m_Renderer = new Renderer(backend, m_Window);

        auto mesh = Mesh(g_Vertices, g_Indices);

        auto material = Material{};
        material.fragmentShader = g_ShaderBasicFragment;
        material.vertexShader = g_ShaderBasicVertex;
        material.vertexAttributes = g_ShaderBasicAttributes;

        auto transform = Transform{};
        transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform.rotation = glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f);
        transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

        m_TestModel = new Model(mesh, material, transform, false);
    }

    Engine::~Engine() {
        delete m_TestModel;

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
                ss << "Nitronic Engine - " << 1.0f / mean << "FPS (" << mean * 1000 << "ms)";
                m_Window->SetTitle(ss.str().c_str());
            }

            Window::PollEvents();

            m_Renderer->BeginScene();

            m_Renderer->RenderModel(m_TestModel);

            m_Renderer->EndScene();

            m_Window->SwapBuffers();
        }
    }

}