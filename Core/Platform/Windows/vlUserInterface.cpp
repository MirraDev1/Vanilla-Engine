#include "vlUserInterface.h"

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
        VlLoadCamCallbacks();
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
