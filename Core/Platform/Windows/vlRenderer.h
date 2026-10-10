#ifndef VL_RENDERER_H
#define VL_RENDERER_H
#include <d3d11.h>
#include <iostream>
#include <wrl/client.h>
#include <memory>
#include <DirectXMath.h>
#include <cstdint>
#include <string>
#include "../../Resources/GpuModel.h"
using namespace Microsoft::WRL;


namespace vl::Resource {
	class Buffer;
}

struct SceneMatrices {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 normal;
};

namespace vl::Platform {
	class Renderer {
	public:
		Renderer();
		~Renderer();
		[[nodiscard]] bool InitRenderer(ComPtr<ID3D11Device>Device,ComPtr<ID3D11RenderTargetView>Rtv, ComPtr<ID3D11DepthStencilView>Dsv, ComPtr<ID3D11DeviceContext>Context, ComPtr<IDXGISwapChain>Swapchain);
		[[nodiscard]] bool InitializeTestTriangle(std::string& error);
		[[nodiscard]] bool InitializeTestCube(std::string& error);
		void DrawTestTriangle(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
		void DrawTestCube(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
		void ReleaseRenderTargets();
		void UpdateRenderTargets(ComPtr<ID3D11RenderTargetView> Rtv, ComPtr<ID3D11DepthStencilView> Dsv);
		void ClearFrame(const float clearColor[4]);
		[[nodiscard]] bool ResizeViewportTarget(std::uint32_t width, std::uint32_t height, std::string& error);
		[[nodiscard]] bool BeginViewport(const float clearColor[4]) noexcept;
		void EndViewport() noexcept;
		[[nodiscard]] ID3D11ShaderResourceView* GetViewportTexture() const noexcept { return viewportShaderResource_.Get(); }
		void DrawMesh(const vl::Resources::GpuMesh& mesh, const DirectX::XMMATRIX& world,
			const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
		void UpdateSceneMatrices(const DirectX::XMMATRIX& world,
			const DirectX::XMMATRIX& view,
			const DirectX::XMMATRIX& projection);
		void Present();
	private:
		int Options = 0;
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		ComPtr<ID3D11RenderTargetView>rtv;
		ComPtr<ID3D11DepthStencilView>dsv;
		ComPtr<ID3D11DeviceContext>context;
		ComPtr<IDXGISwapChain> swapChain;
		ComPtr<ID3D11Device>device;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		ComPtr<ID3D11Buffer> sceneMatrixBuffer;
		ComPtr<ID3D11Buffer> materialBuffer;
		ComPtr<ID3D11Texture2D> viewportColorTexture_;
		ComPtr<ID3D11RenderTargetView> viewportRenderTarget_;
		ComPtr<ID3D11ShaderResourceView> viewportShaderResource_;
		ComPtr<ID3D11Texture2D> viewportDepthTexture_;
		ComPtr<ID3D11DepthStencilView> viewportDepthStencil_;
		vl::Resources::GpuMesh testTriangle_;
		vl::Resources::GpuMesh testCube_;
		std::uint32_t viewportWidth_ = 0;
		std::uint32_t viewportHeight_ = 0;
		std::unique_ptr<vl::Resource::Buffer>constantBuffer;
	};
}

#endif // VL_RENDERER_H
