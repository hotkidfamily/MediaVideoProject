#pragma once

enum LogLevel
{
	System = -1,
	Debug = 0,
	Info,
	Notice,
	Warn,
	Error,
	Fatal,
};

typedef void(*PLOG_CALLBACK_FUNC)(int, const char* format, va_list);

void logger( int lvel, const char *fmt, ... );
