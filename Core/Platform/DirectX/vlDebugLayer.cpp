#include "vlDebugLayer.h"
#include "imgui.h"

namespace vl {
    ComPtr<ID3D11Debug> DebugLayer::debug_ = nullptr;
    vlLogger DebugLayer::logger_{};

    void DebugLayer::vlConsole(){
        if (!consoleOpen_)
            return;

        if (ImGui::Begin("Console", &consoleOpen_)) {
            if(ImGui::Button("Clear")) {
                logger_.EraseLogMsg();
			}

			ImGui::Separator();
            const auto& msg = logger_.GetLogMsg();

            for (const auto& logs : msg) {
				ImGui::Text("%s: %s", logs.IssueType.c_str(), logs.text.c_str());
            }
        }

        ImGui::End();
    }

    bool DebugLayer::Check(HRESULT hr, std::string_view message) {
        if (FAILED(hr)) {
			logger_.Error("VL::DX::ERR", message);
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
