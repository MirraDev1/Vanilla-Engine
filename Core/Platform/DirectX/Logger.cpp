#include "Logger.h"

void vlLogger::Log(std::string_view category, std::string_view text, LogLevel level){
	std::scoped_lock lock(logMutex);
	logMessages.push_back({std::string(category), level, std::string(text)});
	if(logMessages.size() > maxLogMessages){
		logMessages.pop_front();
	}
}

void vlLogger::Info(std::string_view Issue, std::string_view text){
	Log(Issue, text, LogLevel::Info);
}

void vlLogger::Warning(std::string_view Issue, std::string_view text){
	Log(Issue, text, LogLevel::Warning);
}

void vlLogger::Error(std::string_view Issue, std::string_view text){
	Log(Issue, text, LogLevel::Error);
}

std::deque<LogMsg> vlLogger::GetLogMsg() const{
	std::scoped_lock lock(logMutex);
	return { logMessages.begin(),
	         logMessages.end()};
}

void vlLogger::EraseLogMsg(){
	std::scoped_lock lock(logMutex);
	logMessages.clear();
}


