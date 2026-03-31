//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_PSOCACHE_H
#define NITRONIC_PSOCACHE_H

#include "ShaderCache.h"

NAMESPACE {

    struct PSOKey {
        ShaderKey vertexShader;
        ShaderKey fragmentShader;

        nvrhi::RenderState renderState;
        nvrhi::PrimitiveType primType = nvrhi::PrimitiveType::TriangleList;

        std::vector<nvrhi::VertexAttributeDesc> vertexAttributes;
        nvrhi::BindingLayoutHandle bindingLayout;

        nvrhi::FramebufferInfoEx framebufferInfo;

        bool operator==(const PSOKey& o) const noexcept {
            if (vertexAttributes.size() != o.vertexAttributes.size()) return false;
            for (size_t i = 0; i < vertexAttributes.size(); ++i)
                if (std::memcmp(&vertexAttributes[i], &o.vertexAttributes[i],
                                sizeof(nvrhi::VertexAttributeDesc)) != 0) return false;

            return vertexShader     == o.vertexShader
                && fragmentShader   == o.fragmentShader
                && primType         == o.primType
                && bindingLayout == o.bindingLayout
                && framebufferInfo  == o.framebufferInfo
                && std::memcmp(&renderState, &o.renderState, sizeof(nvrhi::RenderState)) == 0;
        }
    };

    struct PSOKeyHash {
        size_t operator()(const PSOKey& k) const noexcept {
            constexpr ShaderKeyHash sh;
            size_t h = sh(k.vertexShader);
            auto mix = [&](const size_t v) {
                h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
            };
            mix(sh(k.fragmentShader));
            mix(std::hash<int>{}(static_cast<int>(k.primType)));
            mix(std::hash<void*>{}(k.bindingLayout.Get()));

            mix(std::hash<int>{}(static_cast<int>(k.framebufferInfo.colorFormats[0])));
            mix(std::hash<int>{}(static_cast<int>(k.framebufferInfo.depthFormat)));

            const auto* rs = reinterpret_cast<const uint8_t*>(&k.renderState);
            for (size_t i = 0; i < sizeof(nvrhi::RenderState); ++i)
                mix(std::hash<uint8_t>{}(rs[i]));

            for (const auto& attr : k.vertexAttributes) {
                mix(std::hash<std::string>{}(attr.name));
                mix(std::hash<int>{}(static_cast<int>(attr.format)));
                mix(std::hash<uint32_t>{}(attr.bufferIndex));
                mix(std::hash<uint32_t>{}(attr.offset));
            }
            return h;
        }
    };

    class PSOCache {
    public:
        PSOCache(nvrhi::IDevice* device, ShaderCache& shaderCache)
            : m_Device(device)
            , m_ShaderCache(shaderCache)
        {
            assert(m_Device && "PSOCache requires a valid device");
        }

        PSOCache(const PSOCache&)            = delete;
        PSOCache& operator=(const PSOCache&) = delete;
        PSOCache(PSOCache&&)                 = delete;
        PSOCache& operator=(PSOCache&&)      = delete;

        nvrhi::GraphicsPipelineHandle getPipeline(const PSOKey& key)
        {
            {
                std::shared_lock lock(m_Mutex);
                if (auto it = m_Cache.find(key); it != m_Cache.end())
                    return it->second;
            }

            nvrhi::GraphicsPipelineHandle pso = build(key);

            {
                std::unique_lock lock(m_Mutex);
                auto [it, inserted] = m_Cache.emplace(key, pso);
                return it->second;
            }
        }

        void invalidateShader(const ShaderKey& shaderKey) {
            std::unique_lock lock(m_Mutex);
            for (auto it = m_Cache.begin(); it != m_Cache.end();) {
                const PSOKey& k = it->first;
                if (k.vertexShader   == shaderKey ||
                    k.fragmentShader == shaderKey)
                {
                    it = m_Cache.erase(it);
                }
                else ++it;
            }
        }

        void clear() {
            std::unique_lock lock(m_Mutex);
            m_Cache.clear();
        }

        size_t size() const {
            std::shared_lock lock(m_Mutex);
            return m_Cache.size();
        }

    private:
        nvrhi::GraphicsPipelineHandle build(const PSOKey &key) const {
            const nvrhi::ShaderHandle vs = resolve(key.vertexShader);
            const nvrhi::ShaderHandle fs = resolve(key.fragmentShader);
            if (!vs || !fs) return nullptr;

            const std::vector<nvrhi::VertexAttributeDesc>& attrs = key.vertexAttributes;

            const nvrhi::GraphicsPipelineDesc desc = nvrhi::GraphicsPipelineDesc()
                .setVertexShader(vs)
                .setFragmentShader(fs)
                .setPrimType(key.primType)
                .setRenderState(key.renderState)
                .setInputLayout(m_Device->createInputLayout(attrs.data(), static_cast<uint32_t>(attrs.size()), vs))
                .addBindingLayout(key.bindingLayout);

            return m_Device->createGraphicsPipeline(desc, key.framebufferInfo);
        }

        nvrhi::ShaderHandle resolve(const ShaderKey& key) const {
            if (key.stage == nvrhi::ShaderType::None) return nullptr;
            return m_ShaderCache.getShader(key);
        }

    private:
        nvrhi::IDevice*   m_Device; // Borrowed from Renderer
        ShaderCache&      m_ShaderCache; // Borrowed from Renderer

        mutable std::shared_mutex m_Mutex;
        std::unordered_map<PSOKey, nvrhi::GraphicsPipelineHandle, PSOKeyHash> m_Cache;
    };

}

#endif //NITRONIC_PSOCACHE_H