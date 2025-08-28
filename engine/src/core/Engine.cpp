//
// Created by simon on 28/08/2025.
//

#include <iostream>
#include <ostream>
#include <engine/Engine.h>

namespace Nitronic {

    Engine::Engine() {
        std::cout << "Hello from constructor" << std::endl;
    }

    Engine::~Engine() {
        std::cout << "Hello from destructor" << std::endl;
    }

    void Engine::Run() {
        std::cout << "Hello from Run()" << std::endl;
    }


}