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
        PSOCache(PSOCache&&)                 = default;
        PSOCache& operator=(PSOCache&&)      = default;

        nvrhi::GraphicsPipelineHandle get(const PSOKey& key)
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
        nvrhi::GraphicsPipelineHandle build(const PSOKey &key) {
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

            // Create a framebuffer temporarily. This is not used for rendering,
            // rather it serves as a way to tell the pipeline the formats used.
            const nvrhi::FramebufferHandle fb = createCompatibleFramebuffer(key.framebufferInfo);
            if (!fb) return nullptr;

            return m_Device->createGraphicsPipeline(desc, fb);
        }

        nvrhi::ShaderHandle resolve(const ShaderKey& key) const {
            if (key.stage == nvrhi::ShaderType::None) return nullptr;
            return m_ShaderCache.getShader(key);
        }

        // Creates a transient framebuffer.
        nvrhi::FramebufferHandle createCompatibleFramebuffer(const nvrhi::FramebufferInfoEx& info) const {
            std::vector<nvrhi::TextureHandle> textures;
            nvrhi::FramebufferDesc desc{};

            for (auto colorFormat : info.colorFormats)
            {
                if (colorFormat == nvrhi::Format::UNKNOWN) break;

                // 1×1 dummy texture — just for the format signature.
                nvrhi::TextureDesc td;
                td.width       = 1;
                td.height      = 1;
                td.format      = colorFormat;
                td.isRenderTarget = true;
                td.debugName   = "PSOCache::dummyColor";

                nvrhi::TextureHandle tex = m_Device->createTexture(td);
                if (!tex) return nullptr;

                nvrhi::FramebufferAttachment attachment = nvrhi::FramebufferAttachment()
                    .setTexture(tex)
                    .setFormat(colorFormat);

                textures.push_back(tex);
                desc.addColorAttachment(attachment);
            }

            if (info.depthFormat != nvrhi::Format::UNKNOWN)
            {
                nvrhi::TextureDesc td;
                td.width       = 1;
                td.height      = 1;
                td.format      = info.depthFormat;
                td.isRenderTarget = true;
                td.debugName   = "PSOCache::dummyDepth";

                nvrhi::TextureHandle tex = m_Device->createTexture(td);
                if (!tex) return nullptr;

                nvrhi::FramebufferAttachment attachment = nvrhi::FramebufferAttachment()
                    .setTexture(tex)
                    .setFormat(info.depthFormat);

                textures.push_back(tex);
                desc.setDepthAttachment(attachment);
            }

            return m_Device->createFramebuffer(desc);
        }

    private:
        nvrhi::IDevice*   m_Device;
        ShaderCache&      m_ShaderCache;

        mutable std::shared_mutex m_Mutex;
        std::unordered_map<PSOKey, nvrhi::GraphicsPipelineHandle, PSOKeyHash> m_Cache;
    };

}

#endif //NITRONIC_PSOCACHE_H