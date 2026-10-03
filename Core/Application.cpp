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
#include <iostream>
#include <algorithm>


vl::App::Application::Application()
{
}

vl::App::Application::~Application()
{
}

void vl::App::Application::vlGetEvents(){
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
	vertexShader_ = std::make_unique<vl::Shaders::VertexShader>();
	pixelShader_ = std::make_unique<vl::Shaders::PixelShader>();

	vertexShader_->InitInstance(core_->GetDevice(), core_->GetContext());
	vertexshader.filepath = "Shaders/VertexShader.hlsl";
	vertexshader.entry = "VSMain";
	vertexshader.shader_model = "vs_5_0";
	vertexShader_->vlshaderinf(vertexshader);

    pixelShader_->InitInstance(core_->GetDevice(), core_->GetContext());
	pixelshader.filename = "Shaders/PixelShader.hlsl";
	pixelshader.entry = "PSain";
	pixelshader.shader_model = "ps_5_0";
	pixelShader_->vlpshaderInf(pixelshader);

	renderer_->InitRenderer(core_->GetDevice(), core_->GetRenderTargetView(), core_->GetDepthStencilView(), core_->GetContext(), core_->GetSwapChain());
	userInterface_->vlInitUI(window_->GetHandle(), core_->GetDevice(), core_->GetContext());

	debugLayer_ = std::make_unique<vl::DebugLayer>();

	userInterface_->RegisterCamCallbacks(std::bind(&vl::DebugLayer::vlConsole, debugLayer_.get()));
}

int vl::App::Application::Run() {
	vlGetEvents();
	while (!glfwWindowShouldClose(window_->GetHandle())) {
       glfwPollEvents();

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
	   renderer_->UpdateSceneMatrices(
		   DirectX::XMMatrixIdentity(),
		   camera_->GetViewMatrix(),
		   camera_->GetProjectionMatrix());

	   userInterface_->vlStageUI();

	   vertexShader_->vlLoadVertexShader(core_->GetDevice(), vertexshader);
	   vertexShader_->vlGetVertexShader(vertexshader);
	   pixelShader_->vlLoadPixelShader(core_->GetDevice(), pixelshader);
	   pixelShader_->vlGetPixelShader(pixelshader);
	   
	   userInterface_->vlRenderUI();

	   renderer_->Present();
    }
   return 0;
}
