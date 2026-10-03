#pragma once
#include <string>
#include <iostream>
#include <mutex>
#include <deque>
#include <string_view>

enum LogLevel {
	Info,
	Warning,
	Error
};

struct LogMsg {
	std::string IssueType;
	LogLevel level;
	std::string text;
};

class vlLogger {
public:
	void Log(std::string_view Issue, std::string_view text, LogLevel level);
	void Info(std::string_view Issue, std::string_view text);
	void Warning(std::string_view Issue, std::string_view text);
	void Error(std::string_view Issue, std::string_view text);
	[[nodiscard]] std::deque<LogMsg> GetLogMsg() const;
	void EraseLogMsg();
private:
	mutable std::mutex logMutex;
	const size_t maxLogMessages = 100;
	std::deque<LogMsg> logMessages;
};
