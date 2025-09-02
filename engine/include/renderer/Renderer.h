//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include "core/Enums.h"
#include "core/Macros.h"
#include <nvrhi/nvrhi.h>

#include "Device.h"
#include "engine/Window.h"

NAMESPACE {

    class Renderer {
    public:
        Renderer(RenderingBackend backend, Window* window);
        ~Renderer();

        void Render(double deltaTime);
    private:
        CREATE_BACKEND_FUNCTIONS(Init)
        CREATE_BACKEND_FUNCTIONS(Render, double deltaTime)
        CREATE_BACKEND_FUNCTIONS(Cleanup)

        RenderingBackend m_Backend;
        Device* m_Device;

        Window* m_Window;
    };

}

#endif //NITRONIC_RENDERER_H