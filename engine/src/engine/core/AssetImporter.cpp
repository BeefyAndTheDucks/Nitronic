//
// Created by simon on 22/03/2026.
//

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/AssetImporter.h"

NAMESPACE {
    
    Mesh* AssetImporter::ImportMesh(const std::filesystem::path& file) {
        const aiScene* scene = m_Importer.ReadFile(file.string(), 
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs
        );
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            throw std::runtime_error("Failed to import mesh");
        }

        const aiMesh* importerMesh = scene->mMeshes[0];

        std::vector<Vertex> vertices;
        for (uint32_t i = 0; i < importerMesh->mNumVertices; i++) {
            vertices.push_back(Vertex{
                {importerMesh->mVertices[i].x, importerMesh->mVertices[i].y, importerMesh->mVertices[i].z},
                {importerMesh->mNormals[i].x, importerMesh->mNormals[i].y, importerMesh->mNormals[i].z},
                {importerMesh->mTextureCoords[0][i].x, importerMesh->mTextureCoords[0][i].y},
            });
        }

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < importerMesh->mNumFaces; i++) {
            const aiFace& face = importerMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        const auto mesh = new Mesh(vertices, indices);

        return mesh;
    }
    
}
