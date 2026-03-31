//
// Created by simon on 29/03/2026.
//

#ifndef NITRONIC_BASICCOMPONENTS_H
#define NITRONIC_BASICCOMPONENTS_H
#include <string>

#include "Transform.h"

NAMESPACE {

    struct GameObject
    {
        std::string name;
        Transform transform = {};
    };

}

#endif //NITRONIC_BASICCOMPONENTS_H