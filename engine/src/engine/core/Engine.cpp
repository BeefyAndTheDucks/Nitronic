//
// Created by simon on 28/08/2025.
//

#define STB_IMAGE_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include <stb_image.h>

#include "engine/Engine.h"

#include <chrono>
#include <numeric>

#include "engine/AssetImporter.h"
#include "renderer/Constants.h"
#include "renderer/Shaders.h"

NAMESPACE {

    Engine::Engine(const int windowWidth, const int windowHeight, const char* windowTitle, RenderingBackend backend)
        : m_TotalTimePassed(0), m_FPSCalcTimePassed(0) {
        m_Window = std::make_unique<Window>(windowWidth, windowHeight, windowTitle);
        m_Renderer = std::make_unique<Renderer>(backend, m_Window.get());

        auto assetImporter = AssetImporter();

        auto monkeyMesh = assetImporter.ImportMesh("../assets/Monkey.obj");
        auto cubeMesh = assetImporter.ImportMesh("../assets/Cube.obj");

        auto material = Material{};
        material.fragmentShader = g_ShaderBasicFragment;
        material.vertexShader = g_ShaderBasicVertex;
        material.vertexAttributes = g_ShaderBasicAttributes;

        auto transform = Transform{};
        transform.position = glm::vec3(1.0f, -1.0f, 0.0f);
        transform.rotation = glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f);
        transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

        auto monkeyModel = std::make_unique<Model>(*monkeyMesh, material, transform, false);

        transform.position = glm::vec3(-1.0f, 1.0f, 0.0f);
        auto cubeModel = std::make_unique<Model>(*cubeMesh, material, transform, false);

        auto camera = Camera{};
        camera.fov = 45.0f;
        camera.transform.position = glm::vec3(0.0f, 0.0f, -5.0f);
        camera.transform.scale = glm::vec3(1.f);
        camera.transform.rotation = glm::quatLookAt(glm::normalize(-camera.transform.position), glm::vec3(0.f,1.f,0.f));

        m_Scene = std::make_unique<Scene>();
        m_Scene->camera = camera;
        m_Scene->AddModel(std::move(monkeyModel));
        m_Scene->AddModel(std::move(cubeModel));
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

            if (m_FPSCalcTimePassed >= 1 && !m_DeltaTimes.empty()) {
                m_FPSCalcTimePassed = 0;
                m_LastMeanDT = std::accumulate(m_DeltaTimes.begin(), m_DeltaTimes.end(), 0.0) / static_cast<double>(m_DeltaTimes.size());
                m_DeltaTimes.clear();
            }

            Window::PollEvents();

            m_Scene->camera.transform.position.z = glm::sin(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
            m_Scene->camera.transform.position.x = glm::cos(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
            m_Scene->camera.transform.position.y = glm::sin(static_cast<float>(m_TotalTimePassed) * 3) * 5.0f;
            m_Scene->camera.transform.rotation = glm::quatLookAt(glm::normalize(-m_Scene->camera.transform.position), glm::vec3(0.f,1.f,0.f));

            m_Renderer->BeginScene(m_Scene->camera);

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
                ImGui::ShowDemoWindow(&m_ShowingDemoWindow);

            ImGui::Begin("Debug");
            ImGui::Text("FPS: %f", 1.0f / deltaTime);
            ImGui::Text("DeltaTime (ms): %f", deltaTime * 1000);

            ImGui::Text("FPS (1s): %f", 1.0f / m_LastMeanDT);
            ImGui::Text("DeltaTime (1s) (ms): %f", m_LastMeanDT * 1000);
            ImGui::End();

            int sign = 1;
            // Update models
            for (const auto& model : m_Scene->models) {
                model->GetMutableTransform()->position.y = sign * glm::sin(static_cast<float>(m_TotalTimePassed) * 7) * 0.25f;
                sign = -sign;
            }

            for (auto& model : m_Scene->models)
                m_Renderer->RenderModel(*model);

            m_Renderer->EndScene();

            m_Window->SwapBuffers();
        }
    }

}