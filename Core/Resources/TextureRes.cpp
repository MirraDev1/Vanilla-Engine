#include "TextureRes.h"

#include <iostream>

namespace {

    HRESULT CreateShaderResourceView(
        ID3D11Device* device,
        const DirectX::ScratchImage& image,
        const DirectX::TexMetadata& metadata,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& destination)
    {
        if (device == nullptr) {
            return E_INVALIDARG;
        }

        return DirectX::CreateShaderResourceView(
            device,
            image.GetImages(),
            image.GetImageCount(),
            metadata,
            destination.ReleaseAndGetAddressOf());
    }

}

namespace vl::Resource {

    HRESULT Texture2D::vlLoadDDS(
        ID3D11Device* device,
        const std::filesystem::path& filename)
    {
        DirectX::ScratchImage image;
        DirectX::TexMetadata metadata{};

        HRESULT hr = DirectX::LoadFromDDSFile(
            filename.c_str(),
            DirectX::DDS_FLAGS_NONE,
            &metadata,
            image);

        if (FAILED(hr)) {
            std::cerr << "Failed to load DDS texture: "
                      << filename.string() << '\n';
            return hr;
        }

        hr = CreateShaderResourceView(device, image, metadata, shaderResourceView_);
        if (FAILED(hr)) {
            std::cerr << "Failed to create DDS shader resource view: "
                      << filename.string() << '\n';
        }

        return hr;
    }

    HRESULT Texture2D::vlLoadWIC(
        ID3D11Device* device,
        const std::filesystem::path& filename)
    {
        DirectX::ScratchImage image;
        DirectX::TexMetadata metadata{};

        HRESULT hr = DirectX::LoadFromWICFile(
            filename.c_str(),
            DirectX::WIC_FLAGS_NONE,
            &metadata,
            image);

        if (FAILED(hr)) {
            std::cerr << "Failed to load WIC texture: "
                      << filename.string() << '\n';
            return hr;
        }

        hr = CreateShaderResourceView(device, image, metadata, shaderResourceView_);
        if (FAILED(hr)) {
            std::cerr << "Failed to create WIC shader resource view: "
                      << filename.string() << '\n';
        }

        return hr;
    }

    HRESULT Texture2D::vlCreateSamplerState(
        ID3D11Device* device,
        D3D11_FILTER filter,
        D3D11_TEXTURE_ADDRESS_MODE addressMode)
    {
        if (device == nullptr) {
            return E_INVALIDARG;
        }

        D3D11_SAMPLER_DESC samplerDescription{};
        samplerDescription.Filter = filter;
        samplerDescription.AddressU = addressMode;
        samplerDescription.AddressV = addressMode;
        samplerDescription.AddressW = addressMode;
        samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDescription.MinLOD = 0.0f;
        samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

        return device->CreateSamplerState(
            &samplerDescription,
            samplerState_.ReleaseAndGetAddressOf());
    }

    void Texture2D::vlBind(
        ID3D11DeviceContext* context,
        UINT textureSlot,
        UINT samplerSlot) const
    {
        if (context == nullptr) {
            return;
        }

        ID3D11ShaderResourceView* view = shaderResourceView_.Get();
        context->PSSetShaderResources(textureSlot, 1, &view);

        ID3D11SamplerState* sampler = samplerState_.Get();
        context->PSSetSamplers(samplerSlot, 1, &sampler);
    }

}
