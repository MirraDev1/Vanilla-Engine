#include "vlProscess.h"
#include "../DirectX/vlDebugLayer.h"
#include "../DirectX/Logger.h"

Proc::Proc()
{
}

bool Proc::LaunchProc(std::filesystem::path& RuntimePath, std::filesystem::path& scenePath, std::filesystem::path& ProjectPath)
{
	si_.cb = sizeof(si_);

	std::wstring commandLine = L"\"" + RuntimePath.wstring() + L"\"";
	commandLine += L" --project \"" + ProjectPath.wstring() + L"\"";
	commandLine += L" --scene \"" + scenePath.wstring() + L"\"";

	std::vector<wchar_t> CommandLine_(commandLine.begin(), commandLine.end());
	CommandLine_.push_back('\0');

	std::filesystem::path current = std::filesystem::current_path();

	BOOL procResult = CreateProcessW(
		NULL,
		CommandLine_.data(),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		current.wstring().c_str(),
		&si_,
		&pi_
	);

	if(!procResult)
	{
		logger_.Error("Error code: " + std::to_string(GetLastError()), "Failed to launch process");
		return false;
	}

	CloseHandle(pi_.hThread);
	return procResult == TRUE;
}
