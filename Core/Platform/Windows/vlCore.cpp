#include "vlCore.h"
#include "vlWindow.h"
#include "../DirectX/vlDebugLayer.h"

namespace vl {
	Core::Core()
	{
	}
	bool Core::VlInitialize(vl::Platform::Window& window) {
		device_ = ComPtr<ID3D11Device>();
		context_ = ComPtr<ID3D11DeviceContext>();
		swapChain_ = ComPtr<IDXGISwapChain>();
		renderTargetView_ = ComPtr<ID3D11RenderTargetView>();
		depthStencilBuffer_ = ComPtr<ID3D11Texture2D>();
		depthStencilView_ = ComPtr<ID3D11DepthStencilView>();
		depthStencilState_ = ComPtr<ID3D11DepthStencilState>();

		HWND hwnd = glfwGetWin32Window(window.GetHandle());

		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferCount = 1;
		scd.BufferDesc.Width = window.Width();
		scd.BufferDesc.Height = window.Height();
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.SampleDesc.Count = 1;
		scd.Windowed = TRUE;
		scd.OutputWindow = hwnd;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;

		UINT createFlags = 0;
#ifdef _DEBUG
		createFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&scd,
			swapChain_.GetAddressOf(),
			device_.GetAddressOf(),
			nullptr,
			context_.GetAddressOf()
		);
		// The app must still run when Windows does not have the optional debug runtime installed.
		if (FAILED(result) && createFlags != 0) {
			result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
				D3D11_SDK_VERSION, &scd, swapChain_.GetAddressOf(), device_.GetAddressOf(), nullptr, context_.GetAddressOf());
		}
		if (FAILED(result)) {
			std::cerr << "VL::Could not create the D3D11 device and swap chain: "
			          << std::hex << result << std::endl;
			return false;
		}

		// Initialize our DebugLayer helper
		vl::DebugLayer::InitDebug(device_);

		ComPtr<ID3D11Texture2D> backBuffer;
		result = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		vl::DebugLayer::Check(result, "Could not get swap chain back buffer");
		if (FAILED(result)) return false;

		result = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf());
		vl::DebugLayer::Check(result, "Could not create RTV");
		if (FAILED(result)) return false;

		// Create depth stencil buffer
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = window.Width();
		depthDesc.Height = window.Height();
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		result = device_->CreateTexture2D(&depthDesc, nullptr, depthStencilBuffer_.GetAddressOf());
		vl::DebugLayer::Check(result, "Failed to create depth stencil buffer");
		if (FAILED(result)) return false;

		// Create depth stencil view
		result = device_->CreateDepthStencilView(depthStencilBuffer_.Get(), nullptr, depthStencilView_.GetAddressOf());
		vl::DebugLayer::Check(result, "Failed to create depth stencil view");
		if (FAILED(result)) return false;

		// Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = false;

		result = device_->CreateDepthStencilState(&dsDesc, depthStencilState_.GetAddressOf());
		vl::DebugLayer::Check(result, "Failed to create depth stencil state");
		if (FAILED(result)) return false;

		context_->OMSetDepthStencilState(depthStencilState_.Get(), 1);

		context_->OMSetRenderTargets(
			1,
			renderTargetView_.GetAddressOf(),
			depthStencilView_.Get()
		);
		return true;
	}

	bool Core::Resize(const unsigned int width, const unsigned int height)
	{
		if (width == 0 || height == 0 || !swapChain_ || !device_ || !context_) {
			return false;
		}

		// The swap chain cannot resize while its back buffer is still bound.
		context_->OMSetRenderTargets(0, nullptr, nullptr);
		context_->Flush();

		renderTargetView_.Reset();
		depthStencilView_.Reset();
		depthStencilBuffer_.Reset();

		HRESULT result = swapChain_->ResizeBuffers(
			0,
			width,
			height,
			DXGI_FORMAT_UNKNOWN,
			0);
		vl::DebugLayer::Check(result, "VL::SwapChain ResizeBuffers Failure!");
		if (FAILED(result)) {
			return false;
		}

		ComPtr<ID3D11Texture2D> backBuffer;
		result = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		vl::DebugLayer::Check(result, "VL::Could not get the resized back buffer!");
		if (FAILED(result)) {
			return false;
		}

		result = device_->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			renderTargetView_.GetAddressOf());
		vl::DebugLayer::Check(result, "VL::Could not create the resized RTV!");
		if (FAILED(result)) {
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

		result = device_->CreateTexture2D(
			&depthDescription,
			nullptr,
			depthStencilBuffer_.GetAddressOf());
		vl::DebugLayer::Check(result, "VL::Could not create the resized depth buffer!");
		if (FAILED(result)) {
			return false;
		}

		result = device_->CreateDepthStencilView(
			depthStencilBuffer_.Get(),
			nullptr,
			depthStencilView_.GetAddressOf());
		vl::DebugLayer::Check(result, "VL::Could not create the resized DSV!");
		if (FAILED(result)) {
			return false;
		}

		context_->OMSetDepthStencilState(depthStencilState_.Get(), 1);
		context_->OMSetRenderTargets(
			1,
			renderTargetView_.GetAddressOf(),
			depthStencilView_.Get());

		return true;
	}

	Core::~Core()
	{
		std::println("VL::Core Resources Released");
	}
}
