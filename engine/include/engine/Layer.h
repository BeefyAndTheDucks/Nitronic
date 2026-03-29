//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_LAYER_H
#define NITRONIC_LAYER_H
#include "core/Macros.h"
#include "../renderer/RendererScene.h"

NAMESPACE {

    // Forward decls
    class Renderer;
    class Window;
    class Engine;

    class Layer
    {
    public:
        explicit Layer(const std::string& debugName = "Unnamed Layer");
        virtual ~Layer() = default;

        virtual void OnAttach(RendererScene& scene) = 0;
        virtual void OnDetach(RendererScene& scene) = 0;
        virtual void OnUpdate(RendererScene& scene, double deltaTimeSeconds) = 0;
        virtual void OnImGuiRender() = 0;

        [[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }
    protected:
        [[nodiscard]] Engine* GetEngine() const { return m_Engine; }
    private:
        friend class Engine;

        std::string m_DebugName;

        Engine* m_Engine;
    };

}


#endif //NITRONIC_LAYER_H