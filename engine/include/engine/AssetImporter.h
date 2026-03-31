//
// Created by simon on 22/03/2026.
//

#ifndef NITRONIC_ASSETIMPORTER_H
#define NITRONIC_ASSETIMPORTER_H

#include <filesystem>

#include <assimp/Importer.hpp>

#include "renderer/Mesh.h"

NAMESPACE {

    class AssetImporter {
    public:
        AssetImporter() = default;
        ~AssetImporter() = default;

        std::shared_ptr<Mesh> ImportMesh(const std::filesystem::path &file);
    private:
        Assimp::Importer m_Importer;
    };

}

#endif //NITRONIC_ASSETIMPORTER_H