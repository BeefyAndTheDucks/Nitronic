//
// Created by simon on 02/09/2025.
//

#include <iostream>

#include "renderer/Renderer.h"
#include "core/Macros.h"

NAMESPACE {

    Renderer::Renderer(const RenderingBackend backend)
        : m_Backend(backend)
    {
        std::cout << "Using backend " << RenderingBackendToString(backend) << std::endl;

        CREATE_BACKEND_SWITCH(Init);
    }

    Renderer::~Renderer() {
        CREATE_BACKEND_SWITCH(Cleanup);
    }

    void Renderer::Render(const double deltaTime) {
        CREATE_BACKEND_SWITCH(Render, deltaTime);
    }
}
