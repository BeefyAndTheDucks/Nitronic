//
// Created by simon on 28/08/2025.
//

#include "engine/AssetImporter.h"
#include "engine/Engine.h"
#include "renderer/Shaders.h"

class EditorLayer : public Nitronic::Layer {
public:
    EditorLayer() : Layer("EditorLayer") {}

    void OnAttach(Nitronic::Scene& scene) override
    {
        auto assetImporter = Nitronic::AssetImporter();

        const auto monkeyMesh = assetImporter.ImportMesh("Monkey.obj");

        auto monkeyMaterial = Nitronic::Material{};
        monkeyMaterial.fragmentShader = Nitronic::g_ShaderBasicFragment;
        monkeyMaterial.vertexShader = Nitronic::g_ShaderBasicVertex;
        monkeyMaterial.vertexAttributes = Nitronic::g_ShaderBasicAttributes;

        auto monkeyTransform = Nitronic::Transform{};

        auto monkeyModel = std::make_unique<Nitronic::Model>(*monkeyMesh, monkeyMaterial, monkeyTransform, false);

        scene.AddModel(std::move(monkeyModel));

        Nitronic::OffscreenFramebufferDesc desc;
        desc.width = 1;
        desc.height = 1;
        desc.debugName = "Game Viewport";
        m_GameFramebuffer = GetEngine()->GetRenderer()->CreateOffscreenFramebuffer(desc);
        GetEngine()->GetRenderer()->Set3DRenderTarget(m_GameFramebuffer.get());
    }

    void OnDetach(Nitronic::Scene& scene) override
    {
        GetEngine()->GetRenderer()->Set3DRenderTarget(nullptr);
        GetEngine()->GetRenderer()->DestroyOffscreenFramebuffer(*m_GameFramebuffer);
        m_GameFramebuffer.reset();
    }

    void OnUpdate(Nitronic::Scene& scene, const double deltaTimeSeconds) override
    {
        m_LastDeltaTime = deltaTimeSeconds;
        m_TotalTimePassed += deltaTimeSeconds;

        scene.camera.transform.position.x = glm::cos(static_cast<float>(m_TotalTimePassed) * 1) * 15.0f;
        scene.camera.transform.position.y = glm::sin(static_cast<float>(m_TotalTimePassed) * 3) * 5.0f;
        scene.camera.transform.position.z = glm::sin(static_cast<float>(m_TotalTimePassed) * 1) * 15.0f;
        scene.camera.transform.rotation = glm::quatLookAt(glm::normalize(-scene.camera.transform.position), glm::vec3(0.f,1.f,0.f));
    }

    void OnImGuiRender() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game");
        const ImVec2 windowSize = ImGui::GetContentRegionAvail();
        if (windowSize.x > 0 && windowSize.y > 0) {
            const auto w = static_cast<uint32_t>(windowSize.x);
            const auto h = static_cast<uint32_t>(windowSize.y);

            if (w != m_GameFramebuffer->GetWidth() || h != m_GameFramebuffer->GetHeight())
                GetEngine()->GetRenderer()->RequestOffscreenResize(*m_GameFramebuffer, w, h);

            const uint32_t idx = GetEngine()->GetRenderer()->GetSwapChainIndex();
            ImGui::Image(m_GameFramebuffer->GetImGuiTexture(idx).textureID, windowSize);
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Statistics");
        ImGui::Text("FPS (60 frames): %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("Delta Time (60 frames): %.2fms", ImGui::GetIO().DeltaTime * 1000);
        ImGui::End();
    }
private:
    double m_TotalTimePassed = 0.0;

    double m_LastDeltaTime = 0.0001;
    std::unique_ptr<Nitronic::OffscreenFramebuffer> m_GameFramebuffer;
};

int main(const int argc, char* argv[]) {
    auto* engine = new Nitronic::Engine(1280, 720, "Nitronic Editor", argc, argv);

    engine->AddLayer(new EditorLayer());

    engine->Run();

    delete engine;
}
