//
// Created by simon on 28/08/2025.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine/Engine.h"

#include <chrono>
#include <iostream>
#include <numeric>

#include "core/AssetImporter.h"
#include "renderer/Constants.h"
#include "renderer/Shaders.h"

NAMESPACE {

    Engine::Engine(const int windowWidth, const int windowHeight, const char* windowTitle, RenderingBackend backend)
        : m_TotalTimePassed(0), m_FPSCalcTimePassed(0) {
        m_Window = new Window(windowWidth, windowHeight, windowTitle);
        m_Renderer = new Renderer(backend, m_Window);

        auto assetImporter = AssetImporter();

        auto monkey = assetImporter.ImportMesh("../assets/Monkey.obj");
        auto cube = assetImporter.ImportMesh("../assets/Cube.obj");

        auto material = Material{};
        material.fragmentShader = g_ShaderBasicFragment;
        material.vertexShader = g_ShaderBasicVertex;
        material.vertexAttributes = g_ShaderBasicAttributes;

        auto transform = Transform{};
        transform.position = glm::vec3(1.0f, -1.0f, 0.0f);
        transform.rotation = glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f);
        transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

        m_TestModel = new Model(*monkey, material, transform, false);

        transform.position = glm::vec3(-1.0f, 1.0f, 0.0f);
        m_TestModel2 = new Model(*cube, material, transform, false);

        m_Camera = Camera{};
        m_Camera.fov = 45.0f;
        m_Camera.transform.position = glm::vec3(0.0f, 0.0f, -5.0f);
        m_Camera.transform.scale = glm::vec3(1.f);
        m_Camera.transform.rotation = glm::quatLookAt(glm::normalize(-m_Camera.transform.position), glm::vec3(0.f,1.f,0.f));
    }

    Engine::~Engine() {
        delete m_TestModel;
        delete m_TestModel2;

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
                m_LastMeanDT = std::accumulate(m_DeltaTimes.begin(), m_DeltaTimes.end(), 0.0) / m_DeltaTimes.size();
                m_DeltaTimes.clear();

                std::stringstream ss;
                ss << "Nitronic Engine - " << 1.0f / m_LastMeanDT << "FPS (" << m_LastMeanDT * 1000 << "ms)";
                m_Window->SetTitle(ss.str().c_str());
            }

            Window::PollEvents();

            m_Camera.transform.position.z = glm::sin(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
            m_Camera.transform.position.x = glm::cos(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
            m_Camera.transform.position.y = glm::sin(static_cast<float>(m_TotalTimePassed) * 3) * 5.0f;
            m_Camera.transform.rotation = glm::quatLookAt(glm::normalize(-m_Camera.transform.position), glm::vec3(0.f,1.f,0.f));

            m_Renderer->BeginScene(m_Camera);

            if (ImGui::BeginMainMenuBar()) {

                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Quit", "Alt+F4")) {
                        m_Window->Close();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem(m_ShowingDemoWindow ? "Close ImGui Demo Window" : "Open ImGui Demo Window")) {
                        m_ShowingDemoWindow = !m_ShowingDemoWindow;
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            if (m_ShowingDemoWindow)
                ImGui::ShowDemoWindow();

            ImGui::Begin("Debug");
            ImGui::Text("FPS: %f", 1.0f / deltaTime);
            ImGui::Text("DeltaTime (ms): %f", deltaTime * 1000);

            ImGui::Text("FPS (0.1s): %f", 1.0f / m_LastMeanDT);
            ImGui::Text("DeltaTime (0.1s) (ms): %f", m_LastMeanDT * 1000);
            ImGui::End();

            m_Renderer->RenderModel(m_TestModel);
            m_Renderer->RenderModel(m_TestModel2);

            m_Renderer->EndScene();

            m_Window->SwapBuffers();
        }
    }

}