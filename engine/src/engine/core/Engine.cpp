//
// Created by simon on 28/08/2025.
//

#include "engine/Engine.h"

#include <chrono>
#include <numeric>

#define STB_IMAGE_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include <stb_image.h>

#include <tracy/Tracy.hpp>

#include "engine/AssetImporter.h"
#include "engine/Log.h"

#include "renderer/Shaders.h"

NAMESPACE {

    Engine::Engine(int windowWidth, int windowHeight, const char* windowTitle, int argc, char* argv[], RenderingBackend backend)
    {
        std::filesystem::current_path(
            std::filesystem::path(argv[0]).parent_path()
        );

        Log::Init();

        ENGINE_INFO("Current path: {}", std::filesystem::current_path().string());

        ENGINE_INFO("Initializing Engine");

        m_Window = std::make_unique<Window>(windowWidth, windowHeight, windowTitle);
        m_Renderer = std::make_unique<Renderer>(backend, m_Window.get());

        m_Scene = std::make_unique<Scene>();

        ENGINE_INFO("Engine Initialized");
    }

    Engine::~Engine()
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach(*m_Scene);
            delete layer;
        }
    }

    void Engine::Run() {
        using clock = std::chrono::high_resolution_clock;
        using time_point = std::chrono::time_point<clock>;

        time_point previousTime = clock::now();

        while (!m_Window->ShouldClose()) {
            ZoneScopedN("Main Loop");

            time_point currentTime = clock::now();
            std::chrono::duration<double> elapsed = currentTime - previousTime;
            const double deltaTime = elapsed.count(); // seconds
            previousTime = currentTime;

            Window::PollEvents();

            {
                ZoneScopedN("Update Layers");

                for (Layer* layer : m_Layers)
                    layer->OnUpdate(*m_Scene, deltaTime);
            }

            m_Renderer->BeginScene(m_Scene->camera);

            {
                ZoneScopedN("Render Models");

                for (auto& model : m_Scene->models)
                    m_Renderer->RenderModel(*model);
            }

            {
                ZoneScopedN("ImGui");

                for (Layer* layer : m_Layers)
                    layer->OnImGuiRender();
            }

            m_Renderer->EndScene();

            m_Window->SwapBuffers();

            FrameMark;
        }
    }

    void Engine::AddLayer(Layer* layer)
    {
        ENGINE_TRACE("Adding layer {} to engine", layer->GetDebugName());
        m_Layers.push_back(layer);
        layer->m_Engine = this;
        layer->OnAttach(*m_Scene);
    }

    void Engine::RemoveLayer(Layer* layer)
    {
        ENGINE_TRACE("Removing layer {} from engine", layer->GetDebugName());

        auto it = std::ranges::find(m_Layers, layer);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
            layer->OnDetach(*m_Scene);
        } else
            ENGINE_WARN("Cannot remove layer {}. (Not found)", layer->GetDebugName());
    }
}
