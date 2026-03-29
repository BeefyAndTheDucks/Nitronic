//
// Created by simon on 22/03/2026.
//

#ifndef NITRONIC_RENDERERSCENE_H
#define NITRONIC_RENDERERSCENE_H
#include "core/Macros.h"
#include "Camera.h"
#include "Model.h"

NAMESPACE {

    struct RendererScene {
        void AddModel(std::unique_ptr<Model> model) { models.push_back(std::move(model)); }

        Camera camera;
        std::vector<std::unique_ptr<Model>> models;
    };

}

#endif //NITRONIC_RENDERERSCENE_H
