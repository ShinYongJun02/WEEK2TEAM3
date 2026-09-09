#include "FLogger.h"

#include <cstdarg>
#include <cstdio>
#include <format>

uint32 FLogger::MaxLogHistoryCount = 20;
TDeque<FLog> FLogger::Logs;

void FLogger::AddLog(const char* Category, ELogLevel Level, const char* Fmt, ...)
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

void FLogger::ClearLog()
{
	Logs.clear();
}
