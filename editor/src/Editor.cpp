//
// Created by simon on 28/08/2025.
//

#include "../../engine/include/engine/Engine.h"

int main(int argc, char *argv[]) {
    auto* engine = new Nitronic::Engine();

    engine->Run();

    delete engine;
}
