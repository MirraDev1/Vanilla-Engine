#include "Application.h"
#include "Platform/Windows/vlWindow.h"
#include "Platform/Windows/vlRenderer.h"
#include "Platform/Windows/vlUserInterface.h"
#include "Platform/DirectX/VertexShader.h"
#include "Platform/DirectX/PixelShader.h"
#include "Platform/Windows/vlCore.h"
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
	VRender renderinf;
	renderinf.viewport = VRENDERER_VIEWPORT_ON;
	renderinf.BgState = VRENDERER_CLEAR_TRUE;
	renderinf.IsobjState = VRENDERER_FILL_WIREFRAME;

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
	pixelshader.entry = "PMain";
	pixelshader.shader_model = "ps_5_0";
	pixelShader_->vlpshaderInf(pixelshader);

	renderer_->InitRenderer(&renderinf, core_->GetDevice(), core_->GetRenderTargetView(), core_->GetDepthStencilView(), core_->GetContext(), core_->GetSwapChain());
	userInterface_->vlInitUI(window_->GetHandle(), core_->GetDevice(), core_->GetContext());
}

int vl::App::Application::Run() {
	vlGetEvents();
	while (!glfwWindowShouldClose(window_->GetHandle())) {
       glfwPollEvents();

	   userInterface_->vlStageUI();

	   vertexShader_->vlLoadVertexShader(core_->GetDevice(), vertexshader);
	   vertexShader_->vlGetVertexShader(vertexshader);
	   pixelShader_->vlLoadPixelShader(core_->GetDevice(), pixelshader);
	   pixelShader_->vlGetPixelShader(pixelshader);

	   renderer_->vlSetRendererInfo();
	   userInterface_->vlRenderUI();

	   renderer_->Present();
    }
   return 0;
}
