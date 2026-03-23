//
// Created by simon on 21/03/2026.
//

#ifndef NITRONIC_SHADERCACHE_H
#define NITRONIC_SHADERCACHE_H
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <unordered_map>

#include "core/Enums.h"
#include "core/Macros.h"
#include "nvrhi/nvrhi.h"
#include "util/IOUtils.h"

NAMESPACE {

    struct ShaderKey {
        std::string shaderName;
        nvrhi::ShaderType stage = nvrhi::ShaderType::None;

        bool operator==(const ShaderKey& o) const noexcept {
            return stage == o.stage && shaderName == o.shaderName;
        }
    };

    struct ShaderKeyHash {
        size_t operator()(const ShaderKey& k) const noexcept {
            size_t h = std::hash<int>{}(static_cast<int>(k.stage));
            h ^= std::hash<std::string>{}(k.shaderName) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    class ShaderCache {
    public:
        ShaderCache(nvrhi::IDevice* device,
                    std::filesystem::path shadersDirectory,
                    RenderingBackend backend)
            : m_Device(device)
            , m_ShadersDir(std::move(shadersDirectory))
            , m_Extension(backend == RenderingBackend::Vulkan ? ".spv" : ".dxil")
        {
            assert(m_Device && "ShaderCache requires a valid device");
        }

        // Disallow copy, allow move.
        ShaderCache(const ShaderCache&)            = delete;
        ShaderCache& operator=(const ShaderCache&) = delete;
        ShaderCache(ShaderCache&&)                 = delete;
        ShaderCache& operator=(ShaderCache&&)      = delete;


        nvrhi::ShaderHandle getShader(const std::string& sourcePath, const nvrhi::ShaderType& stage) {
            return getShader(ShaderKey { sourcePath, stage });
        }

        nvrhi::ShaderHandle getShader(const ShaderKey& key) {
            {
                std::shared_lock lock(m_Mutex);
                if (auto it = m_Shaders.find(key); it != m_Shaders.end())
                    return it->second;
            }

            nvrhi::ShaderHandle shader = load(key);

            {
                std::unique_lock lock(m_Mutex);
                auto [it, inserted] = m_Shaders.emplace(key, std::move(shader));
                return it->second;
            }
        }

        void invalidate(const std::string& shaderName, const nvrhi::ShaderType stage) {
            const ShaderKey key { buildPath(shaderName, stage).string(), stage };
            std::unique_lock lock(m_Mutex);
            m_Shaders.erase(key);
        }

        void clear() {
            std::unique_lock lock(m_Mutex);
            m_Shaders.clear();
        }

        size_t size() const {
            std::shared_lock lock(m_Mutex);
            return m_Shaders.size();
        }

    private:
        std::filesystem::path buildPath(const std::string &shaderName, const nvrhi::ShaderType stage) const {
            return m_ShadersDir / (shaderName + stageSuffix(stage) + m_Extension);
        }

        static std::string stageSuffix(const nvrhi::ShaderType stage) {
            switch (stage) {
                case nvrhi::ShaderType::Vertex:   return ".vertex";
                case nvrhi::ShaderType::Pixel:    return ".fragment";
                case nvrhi::ShaderType::Compute:  return ".compute";
                case nvrhi::ShaderType::Geometry: return ".geometry";
                case nvrhi::ShaderType::Hull:     return ".hull";
                case nvrhi::ShaderType::Domain:   return ".domain";
                default:
                    throw std::runtime_error("ShaderCache: unmapped ShaderType");
            }
        }

        nvrhi::ShaderHandle load(const ShaderKey& key) const {
            const std::filesystem::path path = buildPath(key.shaderName, key.stage);

            if (!std::filesystem::exists(path))
                throw std::runtime_error("ShaderCache: missing blob: " + key.shaderName);

            const std::vector<char> shaderCode = readFile(path);

            const nvrhi::ShaderDesc desc = nvrhi::ShaderDesc()
                .setShaderType(key.stage)
                .setDebugName(key.shaderName);

            return m_Device->createShader(desc, shaderCode.data(), shaderCode.size());
        }

    private:
        nvrhi::IDevice* m_Device; // Borrowed from Renderer

        std::filesystem::path m_ShadersDir;
        std::string m_Extension;

        mutable std::shared_mutex m_Mutex;
        std::unordered_map<ShaderKey, nvrhi::ShaderHandle, ShaderKeyHash> m_Shaders;
    };

}

#endif //NITRONIC_SHADERCACHE_H
