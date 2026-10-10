#include "vlUserInterface.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace vl::UI{
    void UserInterface::vlInitUI(GLFWwindow* window, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context){
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplDX11_Init(device.Get(), context.Get());
    }

    void UserInterface::vlStageUI(){
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::NewFrame();

        vlLoadSettingsPanel();
        vlCreateDockSpace();
        DrawSceneHierarchy();
        DrawInspector();
        DrawViewport();
        VlLoadCamCallbacks();
    }

    void UserInterface::SetScene(vl::Scene::Scene* scene) noexcept
    {
        scene_ = scene;
        selectedEntity_ = 0;
    }

    void UserInterface::RegisterViewportCallbacks(ViewportResizeCallback resize, ViewportTextureCallback texture)
    {
        resizeViewport_ = std::move(resize);
        viewportTexture_ = std::move(texture);
    }

    void UserInterface::DrawSceneHierarchy()
    {
        ImGui::Begin("Scene Hierarchy");
        if (scene_ != nullptr) {
            for (const vl::Scene::EntityId entity : scene_->GetEntities()) {
                const std::string* name = scene_->GetName(entity);
                if (name == nullptr) continue;
                ImGui::PushID(static_cast<int>(entity));
                if (ImGui::Selectable(name->c_str(), selectedEntity_ == entity)) selectedEntity_ = entity;
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void UserInterface::DrawInspector()
    {
        ImGui::Begin("Inspector");
        if (scene_ == nullptr || selectedEntity_ == 0 || !scene_->Contains(selectedEntity_)) {
            ImGui::TextUnformatted("Select an entity to inspect.");
            ImGui::End();
            return;
        }

        if (const std::string* name = scene_->GetName(selectedEntity_)) {
            char nameBuffer[256]{};
            std::strncpy(nameBuffer, name->c_str(), sizeof(nameBuffer) - 1);
            if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
                static_cast<void>(scene_->SetName(selectedEntity_, nameBuffer));
        }

        if (vl::Scene::TransformComponent* transform = scene_->GetTransform(selectedEntity_)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat3("Position", &transform->position.x, 0.05f);
                float rotationDegrees[3] = {
                    DirectX::XMConvertToDegrees(transform->rotation.x),
                    DirectX::XMConvertToDegrees(transform->rotation.y),
                    DirectX::XMConvertToDegrees(transform->rotation.z)
                };
                if (ImGui::DragFloat3("Rotation (degrees)", rotationDegrees, 0.5f)) {
                    transform->rotation = {
                        DirectX::XMConvertToRadians(rotationDegrees[0]),
                        DirectX::XMConvertToRadians(rotationDegrees[1]),
                        DirectX::XMConvertToRadians(rotationDegrees[2])
                    };
                }
                ImGui::DragFloat3("Scale", &transform->scale.x, 0.05f, 0.001f, 1000.0f);
            }
        }

        if (const vl::Scene::MeshComponent* mesh = scene_->GetMesh(selectedEntity_)) {
            ImGui::SeparatorText("Mesh Renderer");
            ImGui::TextUnformatted(mesh->model ? mesh->model->sourcePath.string().c_str() : "No model assigned");
        }
        if (vl::Scene::CameraComponent* camera = scene_->GetCamera(selectedEntity_)) {
            ImGui::SeparatorText("Camera");
            ImGui::DragFloat("Field of view", &camera->fieldOfViewDegrees, 0.25f, 1.0f, 179.0f);
            ImGui::DragFloat("Near plane", &camera->nearPlane, 0.01f, 0.001f, 1000.0f);
            ImGui::DragFloat("Far plane", &camera->farPlane, 1.0f, camera->nearPlane + 0.01f, 1000000.0f);
            ImGui::Checkbox("Active", &camera->active);
        }
        ImGui::End();
    }

    void UserInterface::DrawViewport()
    {
        viewportFocused_ = false;
        viewportHovered_ = false;
        viewportWidth_ = 0;
        viewportHeight_ = 0;
        const bool contentsVisible = ImGui::Begin("Viewport");
        if (contentsVisible) {
            viewportFocused_ = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            viewportHovered_ = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            const ImVec2 available = ImGui::GetContentRegionAvail();
            if (available.x >= 1.0f && available.y >= 1.0f) {
                viewportWidth_ = static_cast<std::uint32_t>(std::lround(available.x));
                viewportHeight_ = static_cast<std::uint32_t>(std::lround(available.y));
                if (resizeViewport_) resizeViewport_(viewportWidth_, viewportHeight_);
                ID3D11ShaderResourceView* texture = viewportTexture_ ? viewportTexture_() : nullptr;
                if (texture != nullptr) {
                    const ImTextureID textureId = static_cast<ImTextureID>(reinterpret_cast<std::uintptr_t>(texture));
                    ImGui::Image(ImTextureRef(textureId), available);
                }
            }
        }
        ImGui::End();
    }

    void UserInterface::vlLoadSettingsPanel(){
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Open Settings", nullptr, &settingsOpen_);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (settingsOpen_) {
            if (ImGui::Begin("Settings", &settingsOpen_, ImGuiWindowFlags_NoCollapse)) {
                ImGui::BeginChild("SettingsNavigation", ImVec2(180.0f, 0.0f), true);
                ImGui::Text("Categories");
                ImGui::Separator();

                for (int i = 0; i < static_cast<int>(settingsCallbacks_.size()); ++i) {
                    auto& panel = settingsCallbacks_[i];
                    if (ImGui::Selectable(panel.MainTitle.c_str(), selectedSettingsPage_ == i)) {
                        selectedSettingsPage_ = i;
                    }
                }
                ImGui::EndChild();

                ImGui::SameLine();
                ImGui::BeginChild("SettingsOptions", ImVec2(0.0f, 0.0f), true);
                if (selectedSettingsPage_ >= 0 &&
                    selectedSettingsPage_ < static_cast<int>(settingsCallbacks_.size())) {
                    auto& panel = settingsCallbacks_[selectedSettingsPage_];
                    ImGui::Text("%s", panel.OtherTitle.c_str());
                    ImGui::Separator();
                    if (panel.Getinf) panel.Getinf();
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }

    }


    void UserInterface::RegisterSettingsCallback(SettingsPanel callback){
		settingsCallbacks_.push_back(std::move(callback));
    }

    void UserInterface::RegisterCamCallbacks(CamCallbacks camcallbacks){
		camCallbacks_.push_back(std::move(camcallbacks));
    }

    void UserInterface::VlLoadCamCallbacks(){
        bool m_camopen = false;
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("View")) {
              ImGui::MenuItem("Controls", nullptr, &m_camopen);
              ImGui::MenuItem("Perspective", nullptr, &m_camopen);
              ImGui::MenuItem("Console", nullptr, &m_camopen);
			  ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        for (auto& camcallback : camCallbacks_) {
            if (camcallback) camcallback();
		}
    }

    void UserInterface::RegisterFileCallbacks(FileCallback filecallbacks){
        FileCallbacks_.push_back(filecallbacks);
    }

    
    void UserInterface::vlSyncFileCallbacks(){
        for (auto& fcallbacks : FileCallbacks_) {
            if (fcallbacks) fcallbacks();
        }
    }

    void UserInterface::vlSyncSettingsCallback(){
		for (auto& panel : settingsCallbacks_) {
			if (panel.Getinf) panel.Getinf();
        }
    }

    void UserInterface::vlRenderUI() {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void UserInterface::vlCreateDockSpace(){
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("Dockspace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dock_id = ImGui::GetID("myDockspace");
        ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }


    UserInterface::~UserInterface(){
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
