#ifndef VL_RENDERER_H
#define VL_RENDERER_H
#include <d3d11.h>
#include <iostream>
#include <wrl/client.h>
#include <memory>
#include <DirectXMath.h>
using namespace Microsoft::WRL;


namespace vl::Resource {
	class Buffer;
}

namespace vl::Platform {
	class Renderer {
	public:
		Renderer();
		~Renderer();
		void InitRenderer(ComPtr<ID3D11Device>Device,ComPtr<ID3D11RenderTargetView>Rtv, ComPtr<ID3D11DepthStencilView>Dsv, ComPtr<ID3D11DeviceContext>Context, ComPtr<IDXGISwapChain>Swapchain);
		void vlSetRendererInfo();
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
		std::unique_ptr<vl::Resource::Buffer>constantBuffer;
	};
}

#endif // VL_RENDERER_H
