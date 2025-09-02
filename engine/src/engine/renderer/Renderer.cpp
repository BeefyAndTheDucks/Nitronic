//
// Created by simon on 02/09/2025.
//

#include <iostream>

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE {

    Renderer::Renderer(const RenderingBackend backend, Window* window)
        : m_Backend(backend), m_Window(window)
    {
        std::cout << "Using backend " << RenderingBackendToString(backend) << std::endl;

        m_Device = new Device(m_Backend);

        CREATE_BACKEND_SWITCH(Init);
    }

    Renderer::~Renderer() {
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        CREATE_BACKEND_SWITCH(Render, deltaTime);
    }
}
