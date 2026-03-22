//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_ENGINE_H
#define NITRONIC_ENGINE_H
#include "core/Macros.h"
#include "renderer/Renderer.h"

#include "Window.h"
#include "core/Enums.h"

NAMESPACE {

    class Engine {
    public:
        Engine(int windowWidth, int windowHeight, const char* windowTitle, RenderingBackend backend = RenderingBackend::Vulkan);
        ~Engine();

        void Run();
    private:
        Window* m_Window;
        Renderer* m_Renderer;

        double m_TotalTimePassed;
        double m_FPSCalcTimePassed;

        double m_LastMeanDT = -1;

        std::vector<double> m_DeltaTimes;

        bool m_ShowingDemoWindow = false;

        Model* m_TestModel;
        Model* m_TestModel2;
        Camera m_Camera;
    };

}

#endif //NITRONIC_ENGINE_H