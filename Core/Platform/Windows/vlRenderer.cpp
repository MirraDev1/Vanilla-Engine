#include "vlRenderer.h"
#include "../DirectX/Buffer.h"
#include "imgui.h"

namespace vl::Platform {

	void Renderer::vlSetRendererInfo(){
		D3D11_VIEWPORT viewport{};
		D3D11_RASTERIZER_DESC rasterizer{};
		rasterizer.CullMode = D3D11_CULL_NONE;
		rasterizer.FrontCounterClockwise = FALSE;
		rasterizer.DepthBias = 0;
		rasterizer.DepthBiasClamp = 0.0f;
		rasterizer.SlopeScaledDepthBias = 0.0f;
		rasterizer.DepthClipEnable = TRUE;
		rasterizer.ScissorEnable = FALSE;
		rasterizer.MultisampleEnable = FALSE;
		rasterizer.AntialiasedLineEnable = FALSE;

		if (render->viewport == VRENDERER_VIEWPORT_ON) {
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = static_cast<FLOAT>(1280);
			viewport.Height = static_cast<FLOAT>(720);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			context->RSSetViewports(1, &viewport);
		}
		else{
			viewport = {};
		}

		if (render->IsobjState == VRENDERER_FILL_SOLID) {
            rasterizer.FillMode = D3D11_FILL_SOLID;
		}
		else {
			rasterizer.FillMode = D3D11_FILL_WIREFRAME;
		}

		rasterizerState = ComPtr<ID3D11RasterizerState>();
		if (SUCCEEDED(device->CreateRasterizerState(&rasterizer, rasterizerState.GetAddressOf()))) {
			context->RSSetState(rasterizerState.Get());
		}

        float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (render->BgState == IsBgCleared::VRENDERER_CLEAR_TRUE) {
		context->ClearRenderTargetView(rtv.Get(), color);
		if (dsv) {
			context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		   }
		}
		else {
			float Color[4] = { 0.0f, 0.0f ,0.0f ,0.0f };
			context->ClearRenderTargetView(rtv.Get(), Color);
		}
	}

	void Renderer::Present() {
		swapChain->Present(1, 0);
	}

	
	void Renderer::SetSceneMatrices(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) {
		if (!sceneMatrixBuffer) return;

		SceneMatrices matrices{};
		DirectX::XMStoreFloat4x4(&matrices.world, DirectX::XMMatrixTranspose(world));
		DirectX::XMStoreFloat4x4(&matrices.view, DirectX::XMMatrixTranspose(view));
		DirectX::XMStoreFloat4x4(&matrices.projection, DirectX::XMMatrixTranspose(projection));
		context->UpdateSubresource(sceneMatrixBuffer.Get(), 0, nullptr, &matrices, 0, 0);
		ID3D11Buffer* buffer = sceneMatrixBuffer.Get();
		context->VSSetConstantBuffers(0, 1, &buffer);
	}

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer() {
	}

	void Renderer::InitRenderer(ComPtr<ID3D11Device>Device, ComPtr<ID3D11RenderTargetView> Rtv, ComPtr<ID3D11DepthStencilView> Dsv, ComPtr<ID3D11DeviceContext> Context, ComPtr<IDXGISwapChain>Swapchain) {
		rtv = std::move(Rtv);
		dsv = std::move(Dsv);
		context = std::move(Context);
		swapChain = std::move(Swapchain);
		device = std::move(Device);
	}
}
