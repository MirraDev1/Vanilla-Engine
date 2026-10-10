#ifndef VL_DEBUGLAYER_H
#define VL_DEBUGLAYER_H

#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <wrl/client.h>
#include <iostream>
#include "Logger.h"
#include <string>
#include <string_view>

using namespace Microsoft::WRL;


namespace vl {
    class DebugLayer {
    public:
        void vlConsole();
        static bool Check(HRESULT hr, std::string_view message);
        static void Log(std::string_view message, LogLevel level = Info);
        static void InitDebug(ComPtr<ID3D11Device> device);
        static void DrainMessages();
        static void ReportLiveObjects();
    private:
        static ComPtr<ID3D11Debug> debug_;
        static ComPtr<ID3D11InfoQueue> infoQueue_;
        static vlLogger logger_;
        bool consoleOpen_ = true;
    };
}

#endif // VL_DEBUGLAYER_H
