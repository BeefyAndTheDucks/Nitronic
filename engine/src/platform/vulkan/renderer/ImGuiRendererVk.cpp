//
// Created by simon on 18/01/2026.
//

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VkMacros.h"
#include "ImGuiRendererVk.h"

#include "VulkanInclude.h"
#include "renderer/Constants.h"

NAMESPACE {

    void ImGuiRenderer::InitVk() {
        m_ImGuiRendererData = new ImGuiRendererDataVk();

        const auto rendererDataVk = RENDERER_DATA;
        const auto deviceDataVk = DEVICE_DATA;

        vk::DescriptorPoolSize poolSizes[] =
        {
            { vk::DescriptorType::eCombinedImageSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
        };

        ImGui_ImplGlfw_InitForVulkan(m_Window->GetNativeWindow(), true);

        uint32_t descriptorPoolSize = 0;
        for (const vk::DescriptorPoolSize& poolSize : poolSizes)
            descriptorPoolSize += poolSize.descriptorCount;

        const auto colorFormat = rendererDataVk->swapChainImageFormat;

        vk::PipelineRenderingCreateInfoKHR pipelineRenderingInfo = vk::PipelineRenderingCreateInfoKHR()
            .setColorAttachmentCount(1)
            .setPColorAttachmentFormats(&colorFormat);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = VK_API_VERSION_1_3;
        initInfo.Instance = rendererDataVk->instance;
        initInfo.PhysicalDevice = deviceDataVk->physicalDevice;
        initInfo.Device = deviceDataVk->logicalDevice;
        initInfo.QueueFamily = deviceDataVk->queueFamilyIndices.graphicsFamily.value();
        initInfo.Queue = deviceDataVk->graphicsQueue;
        initInfo.DescriptorPoolSize = descriptorPoolSize;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = g_MaxFramesInFlight;
        initInfo.Allocator = nullptr;
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoForViewports.PipelineRenderingCreateInfo = pipelineRenderingInfo;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = pipelineRenderingInfo;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void ImGuiRenderer::ShutdownVk() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

    void ImGuiRenderer::BeginFrameVk() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    }

    void ImGuiRenderer::RenderVk(const nvrhi::CommandListHandle &commandList) {
        const auto commandBuffer = static_cast<vk::CommandBuffer>(commandList->getNativeObject(nvrhi::ObjectTypes::VK_CommandBuffer));

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    ImGuiTexture ImGuiRenderer::AddTextureVk(const nvrhi::TextureHandle &texture, const nvrhi::SamplerHandle &sampler) {
        texture->AddRef();
        sampler->AddRef();

        const auto vulkanSampler = static_cast<vk::Sampler>(sampler->getNativeObject(nvrhi::ObjectTypes::VK_Sampler));
        const auto imguiTextureID = reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(
            vulkanSampler, texture->getNativeView(nvrhi::ObjectTypes::VK_ImageView, nvrhi::Format::SBGRA8_UNORM),
            static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal)));

        auto imguiTexture = ImGuiTexture{texture, sampler, imguiTextureID};
        return imguiTexture;
    }

    void ImGuiRenderer::RemoveTextureVk(const ImGuiTexture &texture) {
        ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(texture.textureID));
        texture.texture->Release();
        texture.sampler->Release();
    }

}
