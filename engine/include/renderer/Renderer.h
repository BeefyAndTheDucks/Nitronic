//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_RENDERER_H
#define NITRONIC_RENDERER_H
#include "core/Enums.h"
#include "core/Macros.h"

#include "Device.h"
#include "engine/Window.h"

NAMESPACE {

    struct SwapChainImage {
        //vk::Image image;
        nvrhi::TextureHandle nvrhiHandle;
    };

    struct Vertex {
        float position[3];
        float texCoord[2];
    };

    class Renderer {
    public:
        Renderer(RenderingBackend backend, Window* window);
        ~Renderer();

        void Render(double deltaTime);
    private:
        CREATE_BACKEND_FUNCTIONS(Init)
        CREATE_BACKEND_FUNCTIONS(InitAfterDeviceCreation)
        CREATE_BACKEND_FUNCTIONS(Render, double deltaTime)
        CREATE_BACKEND_FUNCTIONS(CleanupPreDevice)
        CREATE_BACKEND_FUNCTIONS(Cleanup)
    private:
        RenderingBackend m_Backend;
        RendererData* m_RendererData;

        Device* m_Device;
        Window* m_Window;

        std::vector<SwapChainImage> m_SwapChainImages;

        nvrhi::CommandListHandle m_CommandList;
        nvrhi::FramebufferHandle m_Framebuffer;
    };

}

#endif //NITRONIC_RENDERER_H