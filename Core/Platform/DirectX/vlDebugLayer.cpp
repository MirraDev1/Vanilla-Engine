#include "vlDebugLayer.h"
#include "imgui.h"

namespace vl {
    ComPtr<ID3D11Debug> DebugLayer::debug_ = nullptr;

    void DebugLayer::vlConsole(){
        if (!consoleOpen_)
            return;

        if (ImGui::Begin("Console", &consoleOpen_)) {
        }

        ImGui::End();
    }

    bool DebugLayer::Check(HRESULT hr, const std::string& message) {
        if (FAILED(hr)) {
            std::cerr << "VL::DX_ERROR: " << message
                      << " | HRESULT: " << std::hex << hr << std::endl;
        }
        return SUCCEEDED(hr);
    }

    void DebugLayer::InitDebug(ComPtr<ID3D11Device> device) {
#ifdef _DEBUG
        HRESULT hr = device.As(&debug_);
        if (FAILED(hr)) {
            std::cerr << "VL::DebugLayer: Failed to get ID3D11Debug interface.\n";
        }
#endif
    }

    void DebugLayer::ReportLiveObjects() {
        if (debug_) debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
}
