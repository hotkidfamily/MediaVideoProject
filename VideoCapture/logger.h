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



void internel_log( int level, const char *fmt, ... );
