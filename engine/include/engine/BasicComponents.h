//
// Created by simon on 29/03/2026.
//

#ifndef NITRONIC_BASICCOMPONENTS_H
#define NITRONIC_BASICCOMPONENTS_H
#include <string>

#include "renderer/Transform.h"

struct GameObject
{
    std::string& name;
    Nitronic::Transform transform;
};

#endif //NITRONIC_BASICCOMPONENTS_H