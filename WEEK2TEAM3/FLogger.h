#pragma once

#include "Core.h"

#define UE_LOG(Category, Level, ...) FLogger::AddLog(#Category, ELogLevel::##Level, __VA_ARGS__)

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

	static void AddLog(const char* Category, ELogLevel Level, const char* Fmt, ...)
	{
		while (Logs.size() > MaxLogHistoryCount)
		{
			Logs.pop_front();
		}

		// FIXME-OPT
		char Buf[1024];
		va_list Args;
		va_start(Args, Fmt);
		vsnprintf(Buf, sizeof(Buf), Fmt, Args);
		Buf[sizeof(Buf) - 1] = 0;
		va_end(Args);
		
		FLog& Log = Logs.emplace_back();
		Log.Level = Level;
		Log.Timestamp = std::chrono::system_clock::now();
		Log.Category = Category;
		Log.Message = std::format("{}: {}", Category, Buf);
	}

	static void ClearLog()
	{
		Logs.clear();
	}
};

