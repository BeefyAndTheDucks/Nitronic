//
// Created by simon on 22/03/2026.
//

#ifndef NITRONIC_SCENE_H
#define NITRONIC_SCENE_H
#include "core/Macros.h"
#include "renderer/Camera.h"
#include "renderer/Model.h"

NAMESPACE {

    struct Scene {
    public:
        void AddModel(std::unique_ptr<Model> model) { models.push_back(std::move(model)); }

    public:
        Camera camera;
        std::vector<std::unique_ptr<Model>> models;
    };

}

#endif //NITRONIC_SCENE_H
