#pragma once

#include "Core.h"

#define UE_LOG(category, level, ...) FLogger::AddLog(#category, ELogLevel::##level, __VA_ARGS__)

enum class ELogLevel
{
	Info,
	Warning,
	Error
};

struct FLog
{
	ELogLevel Level;
	FDateTime Timestamp;
	FString Category;
	FString Message;
};

struct FLogger
{
	inline static uint32 MaxLogHistoryCount = 20;
	inline static TDeque<FLog> Logs;

	static void AddLog(const char* category, ELogLevel level, const char* fmt, ...)
	{
		while (Logs.size() > MaxLogHistoryCount)
		{
			Logs.pop_front();
		}

		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		buf[sizeof(buf) - 1] = 0;
		va_end(args);
		
		FLog& log = Logs.emplace_back();
		log.Level = level;
		log.Timestamp = std::chrono::system_clock::now();
		log.Category = category;
		log.Message = std::format("{}: {}", category, buf);
	}

	static void ClearLog()
	{
		Logs.clear();
	}
};

