//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_MODEL_H
#define NITRONIC_MODEL_H
#include <nvrhi/nvrhi.h>
#include "Material.h"
#include "Renderer.h"
#include "Transform.h"

NAMESPACE {

    class Model {
    public:
        Model(Vertex vertices[], uint32_t indices[], Material material, Transform transform, bool isStatic);
        ~Model();
    private:
        Transform m_Transform;

        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        nvrhi::BufferHandle m_ObjectConstantsBuffer;
        Material m_Material;
    };

}


#endif //NITRONIC_MODEL_H