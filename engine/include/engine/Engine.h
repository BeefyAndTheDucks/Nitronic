//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_ENGINE_H
#define NITRONIC_ENGINE_H
#include "core/Macros.h"
#include "core/Enums.h"
#include "renderer/Renderer.h"

#include "Window.h"
#include "Scene.h"

NAMESPACE {

    class Engine {
    public:
        Engine(int windowWidth, int windowHeight, const char* windowTitle, RenderingBackend backend = RenderingBackend::Vulkan);
        ~Engine() = default;

        void Run();
    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;

        double m_TotalTimePassed;
        double m_FPSCalcTimePassed;

        std::vector<double> m_DeltaTimes;
        double m_LastMeanDT = -1;

        bool m_ShowingDemoWindow = false;

        std::unique_ptr<Scene> m_Scene;
    };

}

#endif //NITRONIC_ENGINE_H