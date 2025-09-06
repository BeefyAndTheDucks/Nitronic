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

        CREATE_BACKEND_SWITCH(Init);
        m_Device = new Device(m_Backend, m_RendererData);
        CREATE_BACKEND_SWITCH(InitAfterDeviceCreation);
    }

    Renderer::~Renderer() {
        CREATE_BACKEND_SWITCH(CleanupPreDevice);
        delete m_Device;
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        CREATE_BACKEND_SWITCH(Render, deltaTime); // not sure if required. maybe prerender and postrender?
    }
}
