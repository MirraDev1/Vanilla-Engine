#ifndef TEXTURERES_H
#define TEXTURERES_H

#include <d3d11.h>
#include <filesystem>
#include <wrl/client.h>

#include <DirectXTex.h>

namespace vl::Resource {

    // Owns the GPU objects needed to sample one 2D texture.
    class Texture2D {
    public:
        [[nodiscard]] HRESULT vlLoadDDS(
            ID3D11Device* device,
            const std::filesystem::path& filename);

        [[nodiscard]] HRESULT vlLoadWIC(
            ID3D11Device* device,
            const std::filesystem::path& filename);

        [[nodiscard]] HRESULT vlCreateSamplerState(
            ID3D11Device* device,
            D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP);

        // Binds the texture and sampler to the slots used by PixelShader.hlsl.
        void vlBind(ID3D11DeviceContext* context,
                    UINT textureSlot = 0,
                    UINT samplerSlot = 0) const;

        [[nodiscard]] ID3D11ShaderResourceView* GetShaderResourceView() const noexcept
        {
            return shaderResourceView_.Get();
        }

        [[nodiscard]] ID3D11SamplerState* GetSamplerState() const noexcept
        {
            return samplerState_.Get();
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_;
    };

    // Keeps the old short name available while existing engine code is being moved.
    using Texture = Texture2D;
}

#endif // TEXTURERES_H
