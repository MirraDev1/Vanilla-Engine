#include "TextureRes.h"

namespace vl::Resource {
	ComPtr<ID3D11ShaderResourceView> Texture::vlLoadDDS(ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11Device>& device, const wchar_t* filename_) {
		DirectX::ScratchImage image = {};
		DirectX::TexMetadata metadata = {};
		hr = DirectX::LoadFromDDSFile(filename_, DirectX::DDS_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) {
			std::cerr << "\aFailed to load DDS file: " << std::endl;
			return nullptr;
		}

		DirectX::CreateShaderResourceView(
			device.Get(),
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			srv.GetAddressOf()
		);

		return srv.Get();
	}

	ComPtr<ID3D11ShaderResourceView> Texture::vlLoadWIC(ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11Device>& device,const wchar_t* filename_) {
		DirectX::ScratchImage image = {};
		DirectX::TexMetadata metadata = {};
		hr = DirectX::LoadFromWICFile(filename_, DirectX::WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) {
			std::cerr << "\aFailed to load WIC file: " << std::endl;
			return nullptr;
		}

		DirectX::CreateShaderResourceView(
			device.Get(),
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			srv.GetAddressOf()
		);

		return srv.Get();
	}

	void Texture::vlCreateSamplerState(ComPtr<ID3D11SamplerState>& samplerState, ComPtr<ID3D11Device>& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode){
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = filter;
		samplerDesc.AddressU = addressMode;
		samplerDesc.AddressV = addressMode;
		samplerDesc.AddressW = addressMode;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	}
}
