#include "vlRenderer.h"
#include "../DirectX/Buffer.h"
#include "imgui.h"

namespace vl::Platform {

	void Renderer::Present() {
		swapChain->Present(1, 0);
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

		D3D11_BUFFER_DESC matrixDescription{};
		matrixDescription.Usage = D3D11_USAGE_DYNAMIC;
		matrixDescription.ByteWidth = sizeof(SceneMatrices);
		matrixDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		device->CreateBuffer(
			&matrixDescription,
			nullptr,
			sceneMatrixBuffer.GetAddressOf());
	}

	void Renderer::UpdateRenderTargets(
		ComPtr<ID3D11RenderTargetView> Rtv,
		ComPtr<ID3D11DepthStencilView> Dsv) {
		rtv = std::move(Rtv);
		dsv = std::move(Dsv);
	}

	void Renderer::ReleaseRenderTargets() {
		rtv.Reset();
		dsv.Reset();
	}

	void Renderer::ClearFrame(const float clearColor[4]) {
		if (!context || !rtv || clearColor == nullptr) {
			return;
		}

		ID3D11RenderTargetView* renderTarget = rtv.Get();
		context->OMSetRenderTargets(1, &renderTarget, dsv.Get());
		context->ClearRenderTargetView(rtv.Get(), clearColor);

		if (dsv) {
			context->ClearDepthStencilView(
				dsv.Get(),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
				1.0f,
				0);
		}
	}

	void Renderer::UpdateSceneMatrices(const DirectX::XMMATRIX& world,const DirectX::XMMATRIX& view,const DirectX::XMMATRIX& projection) {
		if (!context || !sceneMatrixBuffer) {
			return;
		}

		SceneMatrices matrices{};
		DirectX::XMStoreFloat4x4(&matrices.world,DirectX::XMMatrixTranspose(world));
		DirectX::XMStoreFloat4x4(&matrices.view,DirectX::XMMatrixTranspose(view));
		DirectX::XMStoreFloat4x4(&matrices.projection,DirectX::XMMatrixTranspose(projection));
		context->UpdateSubresource(
			sceneMatrixBuffer.Get(),
			0,
			nullptr,
			&matrices,
			0,
			0);

		ID3D11Buffer* matrixBuffer = sceneMatrixBuffer.Get();
		context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	}
}
