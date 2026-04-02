//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_LAYER_HPP
#define NITRONIC_LAYER_HPP
#include <string>

#include "core/Macros.hpp"

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

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(double deltaTimeSeconds) = 0;
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


#endif //NITRONIC_LAYER_HPP