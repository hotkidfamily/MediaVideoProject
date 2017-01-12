#pragma once

enum LogLevel
{
	Debug = 0,
	Info,
	Notice,
	Warn,
	Error,
	Fatal,
};

typedef void(*PLOG_CALLBACK_FUNC)(int, const char* format, va_list);

void internel_log( int level, const char *fmt, ... );
