//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_OFFSCREENFRAMEBUFFER_HPP
#define NITRONIC_OFFSCREENFRAMEBUFFER_HPP

#include <vector>
#include <string>
#include <nvrhi/nvrhi.h>

#include "core/Macros.hpp"
#include "renderer/ImGuiRenderer.hpp"

NAMESPACE {

    struct OffscreenFramebufferDesc
    {
        uint32_t width = 1;
        uint32_t height = 1;
        nvrhi::Format colorFormat = nvrhi::Format::SBGRA8_UNORM;
        nvrhi::Format depthStencilFormat = nvrhi::Format::D32;
        std::string debugName = "Unnamed Offscreen Framebuffer";
    };

    class OffscreenFramebuffer
    {
    public:
        OffscreenFramebuffer() = default;
        ~OffscreenFramebuffer() = default;

        // Non-copyable, movable
        OffscreenFramebuffer(const OffscreenFramebuffer&) = delete;
        OffscreenFramebuffer& operator=(const OffscreenFramebuffer&) = delete;
        OffscreenFramebuffer(OffscreenFramebuffer&&) = default;
        OffscreenFramebuffer& operator=(OffscreenFramebuffer&&) = default;

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] float GetAspectRatio() const
        {
            return m_Height > 0 ? static_cast<float>(m_Width) / static_cast<float>(m_Height) : 1.0f;
        }
        [[nodiscard]] bool IsValid() const { return m_Width > 0 && m_Height > 0 && !m_Framebuffers.empty(); }

        [[nodiscard]] nvrhi::FramebufferHandle GetFramebuffer(uint32_t swapChainIndex) const
        {
            return m_Framebuffers[swapChainIndex];
        }

        [[nodiscard]] const ImGuiTexture& GetImGuiTexture(uint32_t swapChainIndex) const
        {
            return m_ColorImGuiTextures[swapChainIndex];
        }

    private:
        friend class Renderer; // Renderer manages internals

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        std::string m_DebugName;

        std::vector<nvrhi::FramebufferHandle> m_Framebuffers;
        std::vector<ImGuiTexture> m_ColorImGuiTextures;
        std::vector<nvrhi::TextureHandle> m_DepthStencilTextures;

        nvrhi::Format m_ColorFormat = nvrhi::Format::SBGRA8_UNORM;
        nvrhi::Format m_DepthStencilFormat = nvrhi::Format::D32;
    };

}

#endif //NITRONIC_OFFSCREENFRAMEBUFFER_HPP