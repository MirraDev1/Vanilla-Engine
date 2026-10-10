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
#include <cstdint>
#include "../../Scene/Scene.h"
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
		using ViewportResizeCallback = std::function<bool(std::uint32_t, std::uint32_t)>;
		using ViewportTextureCallback = std::function<ID3D11ShaderResourceView*()>;
		void vlInitUI(GLFWwindow* window, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
		void SetScene(vl::Scene::Scene* scene) noexcept;
		void RegisterViewportCallbacks(ViewportResizeCallback resize, ViewportTextureCallback texture);
		[[nodiscard]] bool IsViewportFocused() const noexcept { return viewportFocused_; }
		[[nodiscard]] bool IsViewportHovered() const noexcept { return viewportHovered_; }
		[[nodiscard]] std::uint32_t GetViewportWidth() const noexcept { return viewportWidth_; }
		[[nodiscard]] std::uint32_t GetViewportHeight() const noexcept { return viewportHeight_; }
		void vlStageUI();
		void vlLoadSettingsPanel();
		void RegisterSettingsCallback(SettingsPanel callback);
		void RegisterCamCallbacks(CamCallbacks camcallbacks);
		void VlLoadCamCallbacks();
		void RegisterFileCallbacks(FileCallback filecallbacks);
		void vlRenderUI();
		void vlCreateDockSpace();
	private:
		void DrawSceneHierarchy();
		void DrawInspector();
		void DrawViewport();
		void vlSyncFileCallbacks();
		void vlSyncSettingsCallback();
		std::vector<SettingsPanel> settingsCallbacks_;
		std::vector<FileCallback>FileCallbacks_;
		std::vector<CamCallbacks>camCallbacks_;
		bool settingsOpen_ = false;
		int selectedSettingsPage_ = 0;
		vl::Scene::Scene* scene_ = nullptr;
		vl::Scene::EntityId selectedEntity_ = 0;
		ViewportResizeCallback resizeViewport_;
		ViewportTextureCallback viewportTexture_;
		std::uint32_t viewportWidth_ = 0;
		std::uint32_t viewportHeight_ = 0;
		bool viewportFocused_ = false;
		bool viewportHovered_ = false;
	};
}






#endif // VL_USERINTERFACE_H
