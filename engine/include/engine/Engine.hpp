//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_ENGINE_HPP
#define NITRONIC_ENGINE_HPP
#include "EventBus.hpp"
#include "Input.hpp"
#include "Layer.hpp"
#include "core/Macros.hpp"
#include "core/Enums.hpp"
#include "renderer/Renderer.hpp"

#include "Window.hpp"

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
        [[nodiscard]] Camera& GetCamera() { return m_CurrentCamera; }

        [[nodiscard]] entt::registry& GetScene() { return m_Scene; }
    private:
        std::unique_ptr<EventBus> m_EventBus;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;

        std::unique_ptr<Input> m_Input;

        std::vector<Layer*> m_Layers;

        Camera m_CurrentCamera;
        entt::registry m_Scene;
    };

}

#endif //NITRONIC_ENGINE_HPP