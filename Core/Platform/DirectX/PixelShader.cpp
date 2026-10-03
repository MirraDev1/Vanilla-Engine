#include "PixelShader.h"
#include "vlDebugLayer.h"
#include <chrono>


    vl::Shaders::PixelShader::PixelShader() = default;

    void vl::Shaders::PixelShader::vlLoadPixelShader(ComPtr<ID3D11Device> device, pshader& shader) {
        using namespace std::chrono_literals;

        if (shader.IsCompiling && shader.is_Compiled.valid()) {
            if (shader.is_Compiled.wait_for(0ms) == std::future_status::ready) {
                pixelCompileResult result = shader.is_Compiled.get();
                shader.IsCompiling = false;
				hr = E_FAIL;

                if (result.success && result.PixelShaderByteCode) {
                    hr = device->CreatePixelShader(
                        result.PixelShaderByteCode->GetBufferPointer(),
                        result.PixelShaderByteCode->GetBufferSize(),
                        nullptr,
                        shader.pixelShader_.GetAddressOf());
                }

                shader.ready = SUCCEEDED(hr);
                shader.errorOpen = !shader.ready;
                if (!shader.ready && result.errorBlob) {
                    shader.errorMessage.assign(
                        static_cast<const char*>(result.errorBlob->GetBufferPointer()),
                        result.errorBlob->GetBufferSize());
                }
                else if (!shader.ready) {
                    shader.errorMessage = "Could not compile or create the pixel shader.";
                }

                if (!shader.ready) {
                    vl::DebugLayer::Check(hr, shader.errorMessage);
                }
            }
        }
    }

    void vl::Shaders::PixelShader::vlpshaderInf(pshader& shader) {
        shader.IsCompiling = true;
        shader.ready = false;
        shader.is_Compiled = std::async(
            std::launch::async,
            &PixelShader::CompilePixelShader,
            this,
            shader.filename,
            shader.entry,
            shader.shader_model);
    }

    void vl::Shaders::PixelShader::vlGetPixelShader(pshader& shader) {
        Context->PSSetShader(shader.pixelShader_.Get(), nullptr, 0);
    }

    pixelCompileResult vl::Shaders::PixelShader::CompilePixelShader(
        std::string_view filePath, std::string entryPoint, std::string shaderModel) {
        pixelCompileResult result{};
        std::wstring filepath(filePath.begin(), filePath.end());

        const HRESULT compileResult = D3DCompileFromFile(
            filepath.c_str(), nullptr, nullptr,
            entryPoint.c_str(), shaderModel.c_str(),
            0, D3DCOMPILE_SKIP_OPTIMIZATION,
            result.PixelShaderByteCode.GetAddressOf(),
            result.errorBlob.GetAddressOf());

        result.success = SUCCEEDED(compileResult);
        return result;
    }

    void vl::Shaders::PixelShader::InitInstance(
        ComPtr<ID3D11Device> deviceInstance,
        ComPtr<ID3D11DeviceContext> context_) {
        device = std::move(deviceInstance);
        Context = std::move(context_);
    }

    vl::Shaders::PixelShader::~PixelShader() = default;
