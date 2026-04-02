//
// Created by simon on 29/03/2026.
//

#ifndef NITRONIC_BASICCOMPONENTS_HPP
#define NITRONIC_BASICCOMPONENTS_HPP
#include <string>

#include "Transform.hpp"

NAMESPACE {

    struct GameObject
    {
        std::string name;
        Transform transform = {};
    };

}

#endif //NITRONIC_BASICCOMPONENTS_HPP