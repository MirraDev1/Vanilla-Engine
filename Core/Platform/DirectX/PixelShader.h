#ifndef PIXEL_SHADER_H
#define PIXEL_SHADER_H
#include <d3d11.h>
#include <wrl/client.h>
#include <string_view>
#include <fstream>
#include <iostream>
#include <future>
#include <d3dcompiler.h>
#include <string>
using namespace Microsoft::WRL;


struct pixelCompileResult {
	ComPtr<ID3DBlob> PixelShaderByteCode = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	bool success = false;
};


struct pshader {
	std::string filename;
	std::string entry;
	std::string shader_model;
	ComPtr<ID3D11PixelShader> pixelShader_;
	std::future<pixelCompileResult> is_Compiled;
	bool IsCompiling = true;
	bool ready = false;
	bool errorOpen = false;
	std::string errorMessage;
};

namespace vl::Shaders {
	class PixelShader {
	public:		
	PixelShader();
	~PixelShader();
	void vlLoadPixelShader(ComPtr<ID3D11Device>device, pshader& pshader);
	void vlpshaderInf(pshader& pshaderinf);
	void vlGetPixelShader(pshader& shader);
	pixelCompileResult CompilePixelShader(std::string_view filePath, std::string entryPoint, std::string shaderModel);
	void InitInstance(ComPtr<ID3D11Device>deviceInstance,ComPtr<ID3D11DeviceContext>context_);
	[[nodiscard]] ID3D11PixelShader* GetPixelShader() const { return pixelShader_.Get(); }
	private:
	ComPtr<ID3D11PixelShader> pixelShader_;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext>Context;
	HRESULT hr = S_OK;
	};
}


#endif PIXEL_SHADER_H
