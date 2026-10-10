#include "vlDebugLayer.h"
#include "imgui.h"
#include <vector>

namespace vl {
    ComPtr<ID3D11Debug> DebugLayer::debug_ = nullptr;
    ComPtr<ID3D11InfoQueue> DebugLayer::infoQueue_ = nullptr;
    vlLogger DebugLayer::logger_{};

    void DebugLayer::vlConsole(){
        if (!consoleOpen_)
            return;

        DrainMessages();
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

    void DebugLayer::Log(std::string_view message, const LogLevel level) {
        logger_.Log("VL::D3D11", message, level);
    }

    void DebugLayer::InitDebug(ComPtr<ID3D11Device> device) {
        if (!device) return;
        HRESULT result = device.As(&debug_);
        if (FAILED(result)) {
            Log("D3D11 debug interfaces unavailable; device was created without the debug runtime.", Warning);
            return;
        }
        result = device.As(&infoQueue_);
        if (FAILED(result)) {
            Log("Could not acquire ID3D11InfoQueue; Direct3D messages will only be available in the debugger output.", Warning);
            return;
        }
        infoQueue_->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue_->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        Log("D3D11 debug layer and info queue are active.", Info);
    }

    void DebugLayer::DrainMessages() {
        if (!infoQueue_) return;
        const UINT64 messageCount = infoQueue_->GetNumStoredMessagesAllowedByRetrievalFilter();
        for (UINT64 index = 0; index < messageCount; ++index) {
            SIZE_T messageSize = 0;
            if (infoQueue_->GetMessage(index, nullptr, &messageSize) != S_FALSE || messageSize == 0) continue;
            std::vector<unsigned char> storage(messageSize);
            auto* message = reinterpret_cast<D3D11_MESSAGE*>(storage.data());
            if (FAILED(infoQueue_->GetMessage(index, message, &messageSize))) continue;
            Log(message->pDescription, message->Severity == D3D11_MESSAGE_SEVERITY_CORRUPTION ||
                message->Severity == D3D11_MESSAGE_SEVERITY_ERROR ? Error :
                message->Severity == D3D11_MESSAGE_SEVERITY_WARNING ? Warning : Info);
        }
        infoQueue_->ClearStoredMessages();
    }

    void DebugLayer::ReportLiveObjects() {
        if (debug_) debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
}
