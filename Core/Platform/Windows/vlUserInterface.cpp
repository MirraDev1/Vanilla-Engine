#include "vlUserInterface.h"
#include <filesystem>
#include "vlProscess.h"
#include "../DirectX/Logger.h"

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
        vlEngineProperties();
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

    
    void UserInterface::vlEngineProperties() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* color = style.Colors;

        style.WindowRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;


        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 6);

        color[ImGuiCol_TabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        color[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);
        color[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.32f, 1.00f);
        color[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.36f, 0.60f, 1.00f);
        color[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.25f, 1.00f);
        color[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.30f, 0.35f, 1.00f);
        color[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.36f, 0.60f, 1.00f);
        color[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        color[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        color[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        color[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        color[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.30f, 0.35f, 1.00f);
        color[ImGuiCol_TabActive] = ImVec4(0.18f, 0.36f, 0.60f, 1.00f);

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f);
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

	//just a simple file explorer to select the scene file and runtime executable
    void UserInterface::run(){
        static char sceneBuffer[256] = "";
		static char runtimeBuffer[256] = "";
        
        std::filesystem::path projectPath = std::filesystem::current_path();

		bool is_open = true;
        if (ImGui::Begin("File Explorer", &is_open)) {
			ImGui::Text("Enter the path to the scene file:");
			ImGui::InputText("Scene Path", sceneBuffer, sizeof(sceneBuffer));
            ImGui::Text("Enter the path to the runtime executable:");
			ImGui::InputText("Runtime Path", runtimeBuffer, sizeof(runtimeBuffer));
        }
		ImGui::End();

        Proc proc;
		bool Toolbar_is_open = true;
        if (ImGui::Begin("Toolbar", &Toolbar_is_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
			float buttonWidth = 40.0f; // Set the desired button width
            if (ImGui::Button("Play", ImVec2(buttonWidth, 0))) {

                std::filesystem::path scenePath = sceneBuffer;
                std::filesystem::path runtimePath = runtimeBuffer;

                if (!proc.LaunchProc(runtimePath, scenePath, projectPath)) {
                    logger_.Warning("VL::Proscess", "Could Not Start Proscess with ID" + std::to_string(proc.GetProcessInfo().dwProcessId));
                    return;
                }
            }
        }
        ImGui::End();
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
