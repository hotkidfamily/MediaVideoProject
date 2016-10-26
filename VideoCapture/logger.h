#pragma once

enum LogLevel
{
	Debug = 0,
	Info,
	Notice,
	Warn,
	Error,
	Fatal,
	NetIo,
};

typedef void (*PVIDEO_CAPTURE_LOG_CALLBACK_FUNC)(int , const char* format, va_list );

void SetLogCallback( PVIDEO_CAPTURE_LOG_CALLBACK_FUNC );
void internel_log( int level, const char *fmt, ... );
