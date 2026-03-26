//
// Created by simon on 28/08/2025.
//

#include "engine/Engine.h"

int main(const int argc, char* argv[]) {
    auto* engine = new Nitronic::Engine(1280, 720, "Nitronic Sandbox", argc, argv);

    engine->Run();

    delete engine;
}
