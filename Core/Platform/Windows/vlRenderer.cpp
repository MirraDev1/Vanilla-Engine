#include "vlRenderer.h"
#include "../DirectX/Buffer.h"
#include "../DirectX/vlDebugLayer.h"
#include "imgui.h"
#include <string>
#include <iostream>

namespace vl::Platform {

	void Renderer::Present() {
		if (swapChain) vl::DebugLayer::Check(swapChain->Present(1, 0), "IDXGISwapChain::Present failed");
	}

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer() {
	}

	bool Renderer::InitRenderer(ComPtr<ID3D11Device>Device, ComPtr<ID3D11RenderTargetView> Rtv, ComPtr<ID3D11DepthStencilView> Dsv, ComPtr<ID3D11DeviceContext> Context, ComPtr<IDXGISwapChain>Swapchain) {
		rtv = std::move(Rtv);
		dsv = std::move(Dsv);
		context = std::move(Context);
		swapChain = std::move(Swapchain);
		device = std::move(Device);

		D3D11_BUFFER_DESC matrixDescription{};
		matrixDescription.Usage = D3D11_USAGE_DEFAULT;
		matrixDescription.ByteWidth = sizeof(SceneMatrices);
		matrixDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		HRESULT result = device->CreateBuffer(
			&matrixDescription,
			nullptr,
			sceneMatrixBuffer.GetAddressOf());
		if (!vl::DebugLayer::Check(result, "CreateBuffer for scene matrices failed")) return false;

		D3D11_BUFFER_DESC materialDescription{};
		materialDescription.Usage = D3D11_USAGE_DEFAULT;
		materialDescription.ByteWidth = sizeof(DirectX::XMFLOAT4);
		materialDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		result = device->CreateBuffer(&materialDescription, nullptr, materialBuffer.GetAddressOf());
		if (!vl::DebugLayer::Check(result, "CreateBuffer for material constants failed")) return false;

		rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = TRUE;
		result = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
		if (!vl::DebugLayer::Check(result, "CreateRasterizerState failed")) return false;
		context->RSSetState(rasterizerState.Get());
		return true;
	}

	bool Renderer::InitializeTestTriangle(std::string& error) {
		vl::Resources::Mesh triangle;
		triangle.name = "Renderer Test Triangle";
		triangle.material.name = "Triangle Gradient";
		triangle.material.diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		triangle.vertices = {
			{ { -1.0f, -0.8f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
			{ {  0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.5f, 0.0f } },
			{ {  1.0f, -0.8f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } }
		};
		triangle.indices = { 0, 1, 2 };
		const bool uploaded = testTriangle_.Upload(device.Get(), triangle, error);
		if (!uploaded) vl::DebugLayer::Log(error, Error);
		return uploaded;
	}

	void Renderer::DrawTestTriangle(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) {
		DrawMesh(testTriangle_, world, view, projection);
	}

	bool Renderer::InitializeTestCube(std::string& error) {
		vl::Resources::Mesh cube;
		cube.name = "Renderer Test Cube";
		cube.material.name = "Default";
		const DirectX::XMFLOAT2 uv[4] = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };
		const auto addFace = [&cube, &uv](const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b,
			const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT3& d) {
			const std::uint32_t first = static_cast<std::uint32_t>(cube.vertices.size());
			cube.vertices.push_back({ a, normal, uv[0] });
			cube.vertices.push_back({ b, normal, uv[1] });
			cube.vertices.push_back({ c, normal, uv[2] });
			cube.vertices.push_back({ d, normal, uv[3] });
			cube.indices.insert(cube.indices.end(), { first, first + 1, first + 2, first, first + 2, first + 3 });
		};
		addFace({ 0.0f, 0.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f });
		addFace({ 0.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f }, { -1.0f, -1.0f, 1.0f });
		addFace({ -1.0f, 0.0f, 0.0f }, { -1.0f, -1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f });
		addFace({ 1.0f, 0.0f, 0.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f });
		addFace({ 0.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f });
		addFace({ 0.0f, -1.0f, 0.0f }, { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, 1.0f }, { -1.0f, -1.0f, 1.0f });
		return testCube_.Upload(device.Get(), cube, error);
	}

	void Renderer::DrawTestCube(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) {
		DrawMesh(testCube_, world, view, projection);
	}

	bool Renderer::ResizeViewportTarget(const std::uint32_t width, const std::uint32_t height, std::string& error) {
		error.clear();
		if (viewportRenderTarget_ && viewportWidth_ == width && viewportHeight_ == height) return true;
		if (!device || width == 0 || height == 0) {
			error = "Viewport render target requires a device and non-zero dimensions";
			return false;
		}

		D3D11_TEXTURE2D_DESC colorDescription{};
		colorDescription.Width = width;
		colorDescription.Height = height;
		colorDescription.MipLevels = 1;
		colorDescription.ArraySize = 1;
		colorDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		colorDescription.SampleDesc.Count = 1;
		colorDescription.Usage = D3D11_USAGE_DEFAULT;
		colorDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		ComPtr<ID3D11Texture2D> colorTexture;
		ComPtr<ID3D11RenderTargetView> renderTarget;
		ComPtr<ID3D11ShaderResourceView> shaderResource;
		HRESULT result = device->CreateTexture2D(&colorDescription, nullptr, colorTexture.GetAddressOf());
		if (SUCCEEDED(result)) result = device->CreateRenderTargetView(colorTexture.Get(), nullptr, renderTarget.GetAddressOf());
		if (SUCCEEDED(result)) result = device->CreateShaderResourceView(colorTexture.Get(), nullptr, shaderResource.GetAddressOf());
		if (FAILED(result)) {
			error = "Failed to create viewport color target (HRESULT " + std::to_string(static_cast<unsigned long>(result)) + ")";
			return false;
		}

		D3D11_TEXTURE2D_DESC depthDescription{};
		depthDescription.Width = width;
		depthDescription.Height = height;
		depthDescription.MipLevels = 1;
		depthDescription.ArraySize = 1;
		depthDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDescription.SampleDesc.Count = 1;
		depthDescription.Usage = D3D11_USAGE_DEFAULT;
		depthDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ComPtr<ID3D11Texture2D> depthTexture;
		ComPtr<ID3D11DepthStencilView> depthStencil;
		result = device->CreateTexture2D(&depthDescription, nullptr, depthTexture.GetAddressOf());
		if (SUCCEEDED(result)) result = device->CreateDepthStencilView(depthTexture.Get(), nullptr, depthStencil.GetAddressOf());
		if (FAILED(result)) {
			error = "Failed to create viewport depth target (HRESULT " + std::to_string(static_cast<unsigned long>(result)) + ")";
			return false;
		}

		viewportColorTexture_ = std::move(colorTexture);
		viewportRenderTarget_ = std::move(renderTarget);
		viewportShaderResource_ = std::move(shaderResource);
		viewportDepthTexture_ = std::move(depthTexture);
		viewportDepthStencil_ = std::move(depthStencil);
		viewportWidth_ = width;
		viewportHeight_ = height;
		return true;
	}

	bool Renderer::BeginViewport(const float clearColor[4]) noexcept {
		if (!context || !viewportRenderTarget_ || !viewportDepthStencil_ || clearColor == nullptr) return false;
		ID3D11RenderTargetView* target = viewportRenderTarget_.Get();
		context->OMSetRenderTargets(1, &target, viewportDepthStencil_.Get());
		context->ClearRenderTargetView(target, clearColor);
		context->ClearDepthStencilView(viewportDepthStencil_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(viewportWidth_);
		viewport.Height = static_cast<float>(viewportHeight_);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);
		return true;
	}

	void Renderer::EndViewport() noexcept {
		if (!context || !rtv) return;
		ID3D11RenderTargetView* target = rtv.Get();
		context->OMSetRenderTargets(1, &target, dsv.Get());
	}

	void Renderer::DrawMesh(const vl::Resources::GpuMesh& mesh, const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) {
		if (!context || !materialBuffer || !mesh.IsReady()) return;
		UpdateSceneMatrices(world, view, projection);
		const DirectX::XMFLOAT4& color = mesh.GetMaterial().diffuseColor;
		context->UpdateSubresource(materialBuffer.Get(), 0, nullptr, &color, 0, 0);
		ID3D11Buffer* colorBuffer = materialBuffer.Get();
		context->PSSetConstantBuffers(1, 1, &colorBuffer);
		mesh.Draw(context.Get());
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
		D3D11_TEXTURE2D_DESC targetDescription{};
		ComPtr<ID3D11Resource> targetResource;
		rtv->GetResource(targetResource.GetAddressOf());
		ComPtr<ID3D11Texture2D> targetTexture;
		if (SUCCEEDED(targetResource.As(&targetTexture))) {
			targetTexture->GetDesc(&targetDescription);
			D3D11_VIEWPORT viewport{};
			viewport.Width = static_cast<float>(targetDescription.Width);
			viewport.Height = static_cast<float>(targetDescription.Height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			context->RSSetViewports(1, &viewport);
		}

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
		const DirectX::XMMATRIX inverseWorld = DirectX::XMMatrixInverse(nullptr, world);
		DirectX::XMStoreFloat4x4(&matrices.normal, DirectX::XMMatrixTranspose(inverseWorld));
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
