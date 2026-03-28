//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_LAYER_H
#define NITRONIC_LAYER_H
#include "core/Macros.h"
#include "Scene.h"

NAMESPACE {

    class Layer
    {
    public:
        explicit Layer(const std::string& debugName = "Unnamed Layer");
        virtual ~Layer() = default;

        virtual void OnAttach(Scene& scene) = 0;
        virtual void OnDetach(Scene& scene) = 0;
        virtual void OnUpdate(Scene& scene, double deltaTimeSeconds) = 0;
        virtual void OnImGuiRender() = 0;

        [[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }
    private:
        std::string m_DebugName;
    };

}


#endif //NITRONIC_LAYER_H