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
	static uint32 MaxLogHistoryCount;
	static TDeque<FLog> Logs;

	static void AddLog(const char* Category, ELogLevel Level, const char* Fmt, ...);
	static void ClearLog();
};
