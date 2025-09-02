//
// Created by simon on 28/08/2025.
//

#include "../../engine/include/engine/Engine.h"

int main(int argc, char *argv[]) {
    auto* engine = new Nitronic::Engine(1280, 720, "Nitronic Editor");

    engine->Run();

    delete engine;
}
