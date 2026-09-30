#ifndef VL_DEBUGLAYER_H
#define VL_DEBUGLAYER_H

#include <d3d11.h>
#include <wrl/client.h>
#include <iostream>
#include <string>

using namespace Microsoft::WRL;

enum shaderType {
    VertexShader,
    PixelShader
};

namespace vl {
    class DebugLayer {
    public:
        static bool Check(HRESULT hr, const std::string& message);
        static void InitDebug(ComPtr<ID3D11Device> device);
        static void ReportLiveObjects();
    private:
        static ComPtr<ID3D11Debug> debug_;
    };
}

#endif // VL_DEBUGLAYER_H
