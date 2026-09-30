#ifndef VANNILA_VERTEXSHADER_H
#define VANNILA_VERTEXSHADER_H
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>
#include <string>
#include <future>
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

struct compileResult {
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3DBlob> VertexShaderByteCode = nullptr;
	bool success = false;
};

struct vshader {
	std::string filepath;
	std::string shader_model;
	std::string entry;
	ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	ComPtr<ID3D11InputLayout> inputLayout = nullptr;
    std::future<compileResult> compile_future;
	bool ready = false;
	bool IsCompiling = true;
	bool errorOpen = false;
	std::string errorMessage;
};

namespace vl::Shaders {
	class VertexShader {
	public:
		VertexShader();
		~VertexShader();
		void vlshaderinf(vshader& shader);
		void vlLoadVertexShader(ComPtr<ID3D11Device>device, vshader& vshaderinf);
		void vlGetVertexShader(vshader& vshader);
		compileResult CompileVertexShader(std::string filename, std::string entry, std::string shader_model);
		void InitInstance(ComPtr<ID3D11Device>deviceInstance,ComPtr<ID3D11DeviceContext>context_);
		[[nodiscard]] ID3D11VertexShader* GetVertexShader() const { return vertexShader.Get(); }
		[[nodiscard]] ID3D11InputLayout* GetLayout() const { return inputLayout.Get(); }
		[[nodiscard]] ID3DBlob* GetVertexShaderByteCode() const { return VertexShaderByteCode.Get(); }
	private:
		void vlGenInputLayout(ComPtr<ID3DBlob> VertexShaderByteCode,UINT bytecodeShaderLength,vshader& ishader, compileResult& cresult);
		ComPtr<ID3D11VertexShader> vertexShader = nullptr;
		ComPtr<ID3DBlob> VertexShaderByteCode = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		ComPtr<ID3D11InputLayout> inputLayout = nullptr;
		ComPtr<ID3D11Device> device;
		HRESULT hr = S_OK;
		ComPtr<ID3D11DeviceContext>Context;
	};
}

#endif VANNILA_VERTEXSHADER_H
