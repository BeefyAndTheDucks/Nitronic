//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_ENGINE_H
#define NITRONIC_ENGINE_H
#include "EventBus.h"
#include "Input.h"
#include "Layer.h"
#include "core/Macros.h"
#include "core/Enums.h"
#include "renderer/Renderer.h"

#include "Window.h"
#include "../renderer/RendererScene.h"

NAMESPACE {

    class Engine {
    public:
        Engine(int windowWidth, int windowHeight, const char* windowTitle, int argc, char* argv[], RenderingBackend backend = RenderingBackend::Vulkan);
        ~Engine();

        void Run();

        void AddLayer(Layer* layer);
        void RemoveLayer(Layer* layer);

        [[nodiscard]] EventBus* GetEventBus() const { return m_EventBus.get(); }
        [[nodiscard]] Renderer* GetRenderer() const { return m_Renderer.get(); }
        [[nodiscard]] Window* GetWindow() const { return m_Window.get(); }

        [[nodiscard]] entt::registry& GetScene() { return m_Scene; }
    private:
        std::unique_ptr<EventBus> m_EventBus;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;

        std::unique_ptr<Input> m_Input;

        std::unique_ptr<RendererScene> m_RenderingScene;

        std::vector<Layer*> m_Layers;

        entt::registry m_Scene;
    };

}

#endif //NITRONIC_ENGINE_H