//
// Created by simon on 29/03/2026.
//

#ifndef NITRONIC_RENDERERCOMPONENTS_H
#define NITRONIC_RENDERERCOMPONENTS_H
#include "Material.h"
#include "Mesh.h"
#include "core/Macros.h"

NAMESPACE {

    struct Rendered
    {
        Mesh* mesh;
        Material* material;
    };

}

#endif //NITRONIC_RENDERERCOMPONENTS_H
