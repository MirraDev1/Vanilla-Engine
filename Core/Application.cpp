#include "Application.h"
#include "Platform/Windows/vlWindow.h"
#include "Platform/Windows/vlRenderer.h"
#include "Platform/Windows/vlUserInterface.h"
#include "Platform/DirectX/VertexShader.h"
#include "Platform/DirectX/PixelShader.h"
#include "Platform/Windows/vlCore.h"
#include "Platform/Windows/vlCam.h"
#include "Platform/Windows/Viewport.h"
#include "Platform/DirectX/vlDebugLayer.h"
#include "Platform/Windows/Input.h"
#include "Scene/Scene.h"
#include "Scene/Systems.h"
#include <iostream>
#include <algorithm>


vl::App::Application::Application()
{
}

vl::App::Application::~Application()
{
}

void vl::App::Application::vlGetEvents(){
	initialized_ = false;
	constexpr unsigned int width = 1280;
	constexpr unsigned int height = 720;
	window_ = std::make_unique<vl::Platform::Window>();
	if (!window_->vlCreateWindow(width, height, "Vanilla Engine")) {
		return;
	}
	core_ = std::make_unique<vl::Core>();
	if (!core_->VlInitialize(*window_)) {
		return;
	}
	camera_ = std::make_unique<Camera>();
	VlViewport initialViewport{
		window_->Width(),
		window_->Height(),
		window_->Width(),
		window_->Height()
	};
	camera_->InitViewport(initialViewport);

	renderer_ = std::make_unique<vl::Platform::Renderer>();
	userInterface_ = std::make_unique<vl::UI::UserInterface>();
	scene_ = std::make_unique<vl::Scene::Scene>();
	testTriangleEntity_ = scene_->CreateEntity("Test Cube");
	vl::Scene::TransformComponent cubeTransform;
	cubeTransform.rotation = { 0.35f, 0.55f, 0.0f };
	scene_->AddTransform(testTriangleEntity_, cubeTransform);
	editorCameraEntity_ = scene_->CreateEntity("Editor Camera");
	vl::Scene::TransformComponent cameraTransform;
	cameraTransform.position = camera_->GetPosition();
	scene_->AddTransform(editorCameraEntity_, cameraTransform);
	vl::Scene::CameraComponent editorCamera;
	editorCamera.active = true;
	scene_->AddCamera(editorCameraEntity_, editorCamera);
	cameraSystem_ = std::make_unique<vl::Scene::CameraSystem>();
	userInterface_->SetScene(scene_.get());
	userInterface_->RegisterViewportCallbacks(
		[this](const std::uint32_t width, const std::uint32_t height) {
			std::string error;
			if (renderer_->ResizeViewportTarget(width, height, error)) return true;
			vl::DebugLayer::Log(error, Warning);
			return false;
		},
		[this]() { return renderer_->GetViewportTexture(); });
	vertexShader_ = std::make_unique<vl::Shaders::VertexShader>();
	pixelShader_ = std::make_unique<vl::Shaders::PixelShader>();

	vertexShader_->InitInstance(core_->GetDevice(), core_->GetContext());
	vertexshader.filepath = "Shaders/VertexShader.hlsl";
	vertexshader.entry = "VSMain";
	vertexshader.shader_model = "vs_5_0";
	vertexShader_->vlshaderinf(vertexshader);

    pixelShader_->InitInstance(core_->GetDevice(), core_->GetContext());
	pixelshader.filename = "Shaders/PixelShader.hlsl";
	pixelshader.entry = "PSMain";
	pixelshader.shader_model = "ps_5_0";
	pixelShader_->vlpshaderInf(pixelshader);

	if (!renderer_->InitRenderer(core_->GetDevice(), core_->GetRenderTargetView(), core_->GetDepthStencilView(), core_->GetContext(), core_->GetSwapChain())) {
		std::cerr << "Renderer initialization failed. See the engine console for Direct3D diagnostics.\n";
		return;
	}
	std::string triangleError;
	if (!renderer_->InitializeTestCube(triangleError)) {
		std::cerr << "Cube test initialization failed: " << triangleError << '\n';
		return;
	}
	if (!renderer_->InitializeTestTriangle(triangleError)) {
		std::cerr << "Triangle test initialization failed: " << triangleError << '\n';
		return;
	}
	userInterface_->vlInitUI(window_->GetHandle(), core_->GetDevice(), core_->GetContext());

	// initialize input (ImGui already initialized inside vlInitUI)
	inputManager_.Init(window_->GetHandle());

	debugLayer_ = std::make_unique<vl::DebugLayer>();

	userInterface_->RegisterCamCallbacks(std::bind(&vl::DebugLayer::vlConsole, debugLayer_.get()));
	initialized_ = true;
}

int vl::App::Application::Run() {
	vlGetEvents();
	if (!initialized_) return -1;
	while (window_ && window_->GetHandle() && !glfwWindowShouldClose(window_->GetHandle())) {
		  glfwPollEvents();

	   userInterface_->vlStageUI();
	   const ImGuiIO& io = ImGui::GetIO();
	   inputManager_.NewFrame(io.WantCaptureKeyboard, io.WantCaptureMouse,
		   userInterface_->IsViewportFocused(), userInterface_->IsViewportHovered());
	   static double previousFrameTime = glfwGetTime();
	   const double currentFrameTime = glfwGetTime();
	   const float deltaTime = static_cast<float>(currentFrameTime - previousFrameTime);
	   previousFrameTime = currentFrameTime;
	   cameraSystem_->Update(*scene_, *camera_, inputManager_, deltaTime);
	   const std::uint32_t viewportWidth = userInterface_->GetViewportWidth();
	   const std::uint32_t viewportHeight = userInterface_->GetViewportHeight();
	   if (viewportWidth > 0 && viewportHeight > 0) {
		   camera_->UpdateViewport({ viewportWidth, viewportHeight, viewportWidth, viewportHeight });
	   }

	   std::uint32_t framebufferWidth = 0;
	   std::uint32_t framebufferHeight = 0;
	   if (window_->ConsumeResize(framebufferWidth, framebufferHeight) &&
		   framebufferWidth > 0 && framebufferHeight > 0) {
		   renderer_->ReleaseRenderTargets();
		   if (core_->Resize(framebufferWidth, framebufferHeight)) {
			   VlViewport resizedViewport{
				   0,
				   0,
				   framebufferWidth,
				   framebufferHeight
			   };
			   camera_->UpdateViewport(resizedViewport);
			   renderer_->UpdateRenderTargets(
				   core_->GetRenderTargetView(),
				   core_->GetDepthStencilView());
		   }
	   }

	   constexpr float clearColor[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	   renderer_->ClearFrame(clearColor);

	   vertexShader_->vlLoadVertexShader(core_->GetDevice(), vertexshader);
	   pixelShader_->vlLoadPixelShader(core_->GetDevice(), pixelshader);
	   if (vertexshader.ready && pixelshader.ready && renderer_->BeginViewport(clearColor)) {
		   vertexShader_->vlGetVertexShader(vertexshader);
		   pixelShader_->vlGetPixelShader(pixelshader);
		   const vl::Scene::TransformComponent* triangleTransform = scene_->GetTransform(testTriangleEntity_);
		   const DirectX::XMMATRIX world = triangleTransform != nullptr ? triangleTransform->GetWorldMatrix() : DirectX::XMMatrixIdentity();
		   renderer_->DrawTestCube(world, camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
		   renderer_->EndViewport();
	   }
	   
	   userInterface_->vlRenderUI();

	   renderer_->Present();
	}

	// on shutdown
	inputManager_.Shutdown();

   return 0;
}
