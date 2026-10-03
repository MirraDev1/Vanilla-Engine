#include "VertexShader.h"
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include "vlDebugLayer.h"

namespace vl::Shaders {
	VertexShader::VertexShader(){

	}

	VertexShader::~VertexShader()
	{
	}

	void VertexShader::vlshaderinf(vshader& shader) {
		shader.IsCompiling = true;
		shader.ready = false;
		shader.compile_future = std::async(std::launch::async, 
			                               &VertexShader::CompileVertexShader,
			                               this, 
			                               shader.filepath, 
			                               shader.entry, 
			                               shader.shader_model);
	}

	void VertexShader::vlLoadVertexShader(ComPtr<ID3D11Device> device, vshader& vshaderinf){
		using namespace std::chrono_literals;

		if (vshaderinf.IsCompiling && vshaderinf.compile_future.valid()) {
			if (vshaderinf.compile_future.wait_for(0ms) == std::future_status::ready) {
				compileResult result = vshaderinf.compile_future.get();
				vshaderinf.IsCompiling = false;
				vshaderinf.errorOpen = false;
				hr = E_FAIL;

				if (result.success && result.VertexShaderByteCode) {
					hr = device->CreateVertexShader(
						result.VertexShaderByteCode->GetBufferPointer(),
						result.VertexShaderByteCode->GetBufferSize(),
						nullptr,
						vshaderinf.vertexShader.GetAddressOf());
				}

				if (SUCCEEDED(hr) && result.VertexShaderByteCode) {
					vlGenInputLayout(result.VertexShaderByteCode, result.VertexShaderByteCode->GetBufferSize(),vshaderinf,result);
					if (!vshaderinf.inputLayout) hr = E_FAIL;
				}

				vshaderinf.ready = SUCCEEDED(hr);
				vshaderinf.errorOpen = !vshaderinf.ready;
				if (!vshaderinf.ready && result.errorBlob) {
					vshaderinf.errorMessage.assign(
						static_cast<const char*>(result.errorBlob->GetBufferPointer()),
						result.errorBlob->GetBufferSize());
				} else if (!vshaderinf.ready) {
					vshaderinf.errorMessage = "Could not compile or create the vertex shader.";
				}

				if (!vshaderinf.ready) {
					vl::DebugLayer::Check(hr, vshaderinf.errorMessage);
				}
			}
		}
	}

	void VertexShader::vlGetVertexShader(vshader& vshader){
		Context->VSSetShader(vshader.vertexShader.Get(), nullptr, 0);
		Context->IASetInputLayout(vshader.inputLayout.Get());
	}

	compileResult VertexShader::CompileVertexShader(std::string filename , std::string entry, std::string shader_model) {
		compileResult cresult{};
		std::wstring filepath(filename.begin(), filename.end());

		HRESULT result = D3DCompileFromFile(
			filepath.c_str(),
			nullptr,
			nullptr,
			entry.c_str(),
			shader_model.c_str(),
			0,
			D3DCOMPILE_SKIP_OPTIMIZATION,
			cresult.VertexShaderByteCode.GetAddressOf(),
			cresult.errorBlob.GetAddressOf());

		cresult.success = SUCCEEDED(result);
		return cresult;
	}

	void VertexShader::vlGenInputLayout(ComPtr<ID3DBlob> VertexShaderByteCode, UINT bytecodeShaderLength,vshader& ishader, compileResult& cresult) {
		if (!VertexShaderByteCode || !device) {
			std::cerr << "VL::Cannot generate input layout without shader bytecode and a device." << std::endl;
			return;
		}

		ComPtr<ID3D11ShaderReflection> shaderReflection = ComPtr<ID3D11ShaderReflection>();
		HRESULT result = D3DReflect(
			VertexShaderByteCode->GetBufferPointer(),
			VertexShaderByteCode->GetBufferSize(),
			IID_PPV_ARGS(shaderReflection.GetAddressOf()));
		if (FAILED(result) || !shaderReflection) {
			std::cerr << "VL::Could not reflect vertex shader: " << std::hex << result << std::endl;
			return;
		}

		D3D11_SHADER_DESC shaderDesc = {};
		result = shaderReflection->GetDesc(&shaderDesc);
		if (FAILED(result)) {
			std::cerr << "VL::Could not read vertex shader description: " << std::hex << result << std::endl;
			return;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputParams;
		inputParams.reserve(shaderDesc.InputParameters);

		for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
			D3D11_SIGNATURE_PARAMETER_DESC ParamDesc = {};
			shaderReflection->GetInputParameterDesc(i, &ParamDesc);

			D3D11_INPUT_ELEMENT_DESC Layout = {};
			Layout.SemanticIndex = ParamDesc.SemanticIndex;
			Layout.SemanticName = ParamDesc.SemanticName;
			Layout.InputSlot = 0;
			Layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			Layout.InstanceDataStepRate = 0;
			Layout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			switch (ParamDesc.Mask) {
			case 1:Layout.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case 3:Layout.Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case 7:Layout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case 15:Layout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			}

			inputParams.push_back(Layout);
		}

		device->CreateInputLayout(
			inputParams.data(),
			(UINT)inputParams.size(),
			cresult.VertexShaderByteCode->GetBufferPointer(),
			cresult.VertexShaderByteCode->GetBufferSize(),
			ishader.inputLayout.GetAddressOf()
		);
	}

	void VertexShader::InitInstance(ComPtr<ID3D11Device> deviceInstance,ComPtr<ID3D11DeviceContext>context_){
		device = deviceInstance;
		Context = context_;
	}
}
