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
        void AddModel(const Model& model) { models.push_back(model); }

    public:
        Camera camera;
        std::vector<Model> models;
    };

}

#endif //NITRONIC_SCENE_H
