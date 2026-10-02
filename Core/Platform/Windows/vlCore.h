#ifndef VL_CORE_H
#define VL_CORE_H
#include <iostream>
#include <d3d11.h>
#include <print>
#include <memory>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace vl::Platform {
	class Window;
}

namespace vl {
	class Core {
	public:
		Core();
		~Core();
		bool VlInitialize(vl::Platform::Window& window);
		bool Resize(unsigned int width, unsigned int height);
		[[nodiscard]] ComPtr<ID3D11Device> GetDevice() const { return device_; }
		[[nodiscard]] ComPtr<ID3D11DeviceContext> GetContext() const { return context_; }
		[[nodiscard]] ComPtr<IDXGISwapChain> GetSwapChain() const { return swapChain_; }
		[[nodiscard]] ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return renderTargetView_; }
		[[nodiscard]] ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return depthStencilView_; }
		[[nodiscard]] ComPtr<ID3D11DepthStencilState> GetDepthStencilState() const { return depthStencilState_; }
		[[nodiscard]] ComPtr<ID3D11Texture2D> GetDepthStencilBuffer() const { return depthStencilBuffer_; }
	private:
		ComPtr<ID3D11Device> device_;
		ComPtr<ID3D11DeviceContext> context_;
		ComPtr<IDXGISwapChain> swapChain_;
		ComPtr<ID3D11RenderTargetView> renderTargetView_;
		ComPtr<ID3D11Texture2D> depthStencilBuffer_;
		ComPtr<ID3D11DepthStencilView> depthStencilView_;
		ComPtr<ID3D11DepthStencilState> depthStencilState_;
		HRESULT hr = S_OK;
	};
}







#endif // VL_CORE_H
