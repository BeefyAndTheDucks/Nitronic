//
// Created by simon on 28/08/2025.
//

#ifndef NITRONIC_ENGINE_H
#define NITRONIC_ENGINE_H
#include "Window.h"

namespace Nitronic {

    class Engine {
    public:
        Engine();
        ~Engine();

        void Run();
    private:
        Window* m_Window;
    };

}

#endif //NITRONIC_ENGINE_H