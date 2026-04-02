//
// Created by simon on 28/08/2025.
//

#include "engine/AssetImporter.h"
#include "engine/BasicComponents.h"
#include "engine/Engine.h"
#include "renderer/Shaders.h"

using namespace Nitronic;

class EditorLayer : public Layer {
public:
    EditorLayer() : Layer("EditorLayer") {}

    void OnAttach() override
    {
        auto assetImporter = AssetImporter();

        const std::shared_ptr<Mesh> monkeyMesh = assetImporter.ImportMesh("Monkey.obj");

        const auto monkeyMaterial = std::make_shared<Material>(g_ShaderBasicFragment, g_ShaderBasicVertex, g_ShaderBasicAttributes);

        auto monkeyTransform = Transform{};

        const entt::entity monkey1 = GetEngine()->GetScene().create();
        const entt::entity monkey2 = GetEngine()->GetScene().create();
        const entt::entity monkey3 = GetEngine()->GetScene().create();
        const entt::entity monkey4 = GetEngine()->GetScene().create();

        GetEngine()->GetScene().emplace<Rendered>(monkey1);
        GetEngine()->GetScene().emplace<Rendered>(monkey2);
        GetEngine()->GetScene().emplace<Rendered>(monkey3);
        GetEngine()->GetScene().emplace<Rendered>(monkey4);

        auto monkeyView = GetEngine()->GetScene().view<Rendered>();
        for (const auto monkey : monkeyView) {
            auto [rendered] = monkeyView.get(monkey);
            rendered.mesh = monkeyMesh;
            rendered.material = monkeyMaterial;
            rendered.cullBackfaces = true;
        }

        auto &[monkey1EntityName, monkeyEntity1Transform] = GetEngine()->GetScene().get<GameObject>(monkey1);
        monkey1EntityName = "Monkey 1";
        monkeyEntity1Transform.position.x = -2;
        monkeyEntity1Transform.position.y = 2;
        monkeyEntity1Transform.position.z = -10;

        auto &[monkey2EntityName, monkeyEntity2Transform] = GetEngine()->GetScene().get<GameObject>(monkey2);
        monkey2EntityName = "Monkey 2";
        monkeyEntity2Transform.position.x = 2;
        monkeyEntity2Transform.position.y = 2;
        monkeyEntity2Transform.position.z = -10;

        auto &[monkey3EntityName, monkeyEntity3Transform] = GetEngine()->GetScene().get<GameObject>(monkey3);
        monkey3EntityName = "Monkey 3";
        monkeyEntity3Transform.position.x = 2;
        monkeyEntity3Transform.position.y = -2;
        monkeyEntity3Transform.position.z = -10;

        auto &[monkey4EntityName, monkeyEntity4Transform] = GetEngine()->GetScene().get<GameObject>(monkey4);
        monkey4EntityName = "Monkey 4";
        monkeyEntity4Transform.position.x = -2;
        monkeyEntity4Transform.position.y = -2;
        monkeyEntity4Transform.position.z = -10;

        OffscreenFramebufferDesc desc;
        desc.width = 1;
        desc.height = 1;
        desc.debugName = "Game Viewport";
        m_GameFramebuffer = GetEngine()->GetRenderer()->CreateOffscreenFramebuffer(desc);
        GetEngine()->GetRenderer()->Set3DRenderTarget(m_GameFramebuffer.get());
    }

    void OnDetach() override
    {
        GetEngine()->GetRenderer()->Set3DRenderTarget(nullptr);
        GetEngine()->GetRenderer()->DestroyOffscreenFramebuffer(*m_GameFramebuffer);
        m_GameFramebuffer.reset();
    }

    void OnUpdate(const double deltaTimeSeconds) override
    {
        m_IsUsingCursorForControls = false;

        m_LastDeltaTime = deltaTimeSeconds;
        m_TotalTimePassed += deltaTimeSeconds;

        if (!m_ViewportFocused) {
            GetEngine()->GetWindow()->SetCursorMode(CursorMode::Normal);
            return;
        }

        const auto dt = static_cast<float>(deltaTimeSeconds);
        auto& cam = GetEngine()->GetCamera().transform;

        if (Input::MouseButtonDown(MouseButton::Right))
        {
            m_IsUsingCursorForControls = true;

            double dx, dy;
            Input::MouseDelta(dx, dy);

            if (dx != 0 || dy != 0)
            {
                m_Yaw += static_cast<float>(dx) * m_MouseSensitivity;
                m_Pitch -= static_cast<float>(dy) * m_MouseSensitivity;
                m_Pitch = glm::clamp(m_Pitch, -89.9f, 89.9f);

                cam.rotation = glm::quatLookAt(m_CameraForward, m_CameraUp);

                m_CameraForward = glm::normalize(glm::vec3(
                    glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch)),
                    glm::sin(glm::radians(m_Pitch)),
                    glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch))
                ));
                m_CameraRight = glm::normalize(glm::cross(m_CameraForward, glm::vec3(0.0f, 1.0f, 0.0f)));
                m_CameraUp    = glm::normalize(glm::cross(m_CameraRight, m_CameraForward));
            }

            glm::vec3 moveDir(0.0f);

            if (Input::MouseButtonDown(MouseButton::Right)) {
                if (Input::KeyDown(Key::W)) moveDir += m_CameraForward;
                if (Input::KeyDown(Key::S)) moveDir -= m_CameraForward;
                if (Input::KeyDown(Key::D)) moveDir += m_CameraRight;
                if (Input::KeyDown(Key::A)) moveDir -= m_CameraRight;
                if (Input::KeyDown(Key::E)) moveDir += m_CameraUp;
                if (Input::KeyDown(Key::Q)) moveDir -= m_CameraUp;
            }

            float moveMultiplier = 1.0f;
            if (Input::KeyDown(Key::LeftShift)) moveMultiplier = 2.0f;

            double scrollX, scrollY;
            Input::MouseScrollDelta(scrollX, scrollY);
            m_MoveSpeedMultiplier += static_cast<float>(scrollY) * m_MoveSpeedMultiplier * 0.1f;
            if (scrollY != 0.0f)
                APP_TRACE("Move speed: {}", m_MoveSpeedMultiplier);

            if (glm::length(moveDir) > 0.0f)
                cam.position += glm::normalize(moveDir) * m_MoveSpeed * moveMultiplier * m_MoveSpeedMultiplier * dt;
        } else
        {
            double scrollX, scrollY;
            Input::MouseScrollDelta(scrollX, scrollY);
            cam.position += cam.rotation * glm::vec3(0.0f, 0.0f, -scrollY);
        }

        GetEngine()->GetWindow()->SetCursorMode(m_IsUsingCursorForControls ? CursorMode::Disabled : CursorMode::Normal);
    }

    void OnImGuiRender() override
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Quit", "Alt+F4")) {
                    GetEngine()->GetWindow()->Close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("Show/Hide Demo Window")) {
                    m_ShowDemoWindow = !m_ShowDemoWindow;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (m_ShowDemoWindow)
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game");

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::SetWindowFocus();

        m_ViewportFocused = ImGui::IsWindowFocused();

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

        ImGui::Begin("Entities");

        const auto view = GetEngine()->GetScene().view<GameObject>();
        for (auto entity : view) {
            auto [gameObject] = view.get(entity);

            // Temporary
            ImGui::TextUnformatted(gameObject.name.c_str());
        }

        ImGui::End();
    }
private:
    double m_TotalTimePassed = 0.0;

    double m_LastDeltaTime = 0.0001;
    std::unique_ptr<OffscreenFramebuffer> m_GameFramebuffer;

    bool m_ViewportFocused = false;

    float m_Yaw                 = -90.0f;
    float m_Pitch               =   0.0f;
    float m_MouseSensitivity    =   0.1f;
    float m_MoveSpeed           =   5.0f;
    float m_MoveSpeedMultiplier =   1.0f;

    glm::vec3 m_CameraForward = {0.0f, 0.0f, -1.0f};
    glm::vec3 m_CameraRight   = {1.0f, 0.0f,  0.0f};
    glm::vec3 m_CameraUp      = {0.0f, 1.0f,  0.0f};

    bool m_IsUsingCursorForControls = false;

    bool m_ShowDemoWindow = false;

    std::vector<entt::entity> m_SelectedEntities;
};

int main(const int argc, char* argv[]) {
    auto* engine = new Engine(1280, 720, "Nitronic Editor", argc, argv);

    engine->AddLayer(new EditorLayer());

    engine->Run();

    delete engine;
}
