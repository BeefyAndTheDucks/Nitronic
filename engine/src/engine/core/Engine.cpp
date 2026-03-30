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

        ENGINE_TRACE("Current path: {}", std::filesystem::current_path().string());

        m_EventBus = std::make_unique<EventBus>();
        m_Window = std::make_unique<Window>(windowWidth, windowHeight, windowTitle, *m_EventBus);
        m_Renderer = std::make_unique<Renderer>(backend, m_Window.get(), *m_EventBus);

        m_Input = std::make_unique<Input>(*m_EventBus);

        m_RenderingScene = std::make_unique<RendererScene>();

        ENGINE_TRACE("Engine Initialized");
    }

    Engine::~Engine()
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach(*m_RenderingScene);
            delete layer;
        }
    }

    void Engine::Run() {
        using clock = std::chrono::high_resolution_clock;
        using time_point = std::chrono::time_point<clock>;

        time_point previousTime = clock::now();

        while (!m_Window->ShouldClose()) {
            ZoneScopedN("Main Loop");

            {
                ZoneScopedN("Flush event queue");

                m_EventBus->flush();
            }

            time_point currentTime = clock::now();
            std::chrono::duration<double> elapsed = currentTime - previousTime;
            const double deltaTime = elapsed.count(); // seconds
            previousTime = currentTime;

            m_Window->PollEvents();

            {
                ZoneScopedN("Update Layers");

                for (Layer* layer : m_Layers)
                    layer->OnUpdate(*m_RenderingScene, deltaTime);
            }

            m_Renderer->BeginScene(m_RenderingScene->camera);

            {
                ZoneScopedN("Render Models");

                for (auto& model : m_RenderingScene->models)
                    m_Renderer->RenderModel(*model);
            }

            {
                ZoneScopedN("ImGui");

                for (Layer* layer : m_Layers)
                    layer->OnImGuiRender();
            }

            m_Renderer->EndScene();

            m_Input->EndFrame();

            FrameMark;
        }
    }

    void Engine::AddLayer(Layer* layer)
    {
        ENGINE_TRACE("Adding layer {} to engine", layer->GetDebugName());
        m_Layers.push_back(layer);
        layer->m_Engine = this;
        layer->OnAttach(*m_RenderingScene);
    }

    void Engine::RemoveLayer(Layer* layer)
    {
        ENGINE_TRACE("Removing layer {} from engine", layer->GetDebugName());

        auto it = std::ranges::find(m_Layers, layer);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
            layer->OnDetach(*m_RenderingScene);
        } else
            ENGINE_WARN("Cannot remove layer {}. (Not found)", layer->GetDebugName());
    }
}
