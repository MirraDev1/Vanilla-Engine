#pragma once
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <vector>

class Proc {
public:
	Proc();
	~Proc(){}
	bool LaunchProc(std::filesystem::path& RuntimePath,std::filesystem::path& scenePath, std::filesystem::path& ProjectPath);
	[[nodiscard]] _PROCESS_INFORMATION GetProcessInfo() const { return pi_; }
private:
	STARTUPINFOW si_;
	_PROCESS_INFORMATION pi_;
	vlLogger& logger_;
};