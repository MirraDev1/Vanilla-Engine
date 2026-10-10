#ifndef VL_USERINTERFACE_H
#define VL_USERINTERFACE_H
#include <d3d11.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <memory>
#include <wrl/client.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_glfw.h>
#include <functional>
#include <iostream>
#include <vector>
using namespace Microsoft::WRL;


struct SettingsPanel {
	std::function<void()> Getinf;
	std::string MainTitle;
	std::string OtherTitle;
	int numPages{};
};

namespace vl::UI {
	class UserInterface {
	public:
		UserInterface() = default;
		~UserInterface();
		using FileCallback = std::function<void()>;
		using CamCallbacks = std::function<void()>;
		void vlInitUI(GLFWwindow* window, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
		void vlStageUI();
		void vlLoadSettingsPanel();
		void RegisterSettingsCallback(SettingsPanel callback);
		void RegisterCamCallbacks(CamCallbacks camcallbacks);
		void VlLoadCamCallbacks();
		void RegisterFileCallbacks(FileCallback filecallbacks);
		void vlRenderUI();
		void run();
		void vlCreateDockSpace();
	private:
		void vlEngineProperties();
		void vlSyncFileCallbacks();
		void vlSyncSettingsCallback();
		std::vector<SettingsPanel> settingsCallbacks_;
		std::vector<FileCallback>FileCallbacks_;
		std::vector<CamCallbacks>camCallbacks_;
		bool settingsOpen_ = false;
		int selectedSettingsPage_ = 0;
		vlLogger& logger_;
	};
}






#endif // VL_USERINTERFACE_H
