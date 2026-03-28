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
    }

    void OnDetach(Nitronic::Scene& scene) override
    {

    }

    void OnUpdate(Nitronic::Scene& scene, double deltaTimeSeconds) override
    {
        m_TotalTimePassed += deltaTimeSeconds;

        scene.camera.transform.position.z = glm::sin(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
        scene.camera.transform.position.x = glm::cos(static_cast<float>(m_TotalTimePassed) * 1) * 5.0f;
        scene.camera.transform.position.y = glm::sin(static_cast<float>(m_TotalTimePassed) * 3) * 5.0f;
        scene.camera.transform.rotation = glm::quatLookAt(glm::normalize(-scene.camera.transform.position), glm::vec3(0.f,1.f,0.f));
    }

    void OnImGuiRender() override
    {

    }
private:
    double m_TotalTimePassed = 0.0;
};

int main(const int argc, char* argv[]) {
    auto* engine = new Nitronic::Engine(1280, 720, "Nitronic Editor", argc, argv);

    engine->AddLayer(new EditorLayer());

    engine->Run();

    delete engine;
}
