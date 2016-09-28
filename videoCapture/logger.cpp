#include "stdafx.h"
#include "logger.h"


logger::logger(STRING filePath)
{
	logfile.open(filePath);
}


logger::~logger()
{
}


void logger::log(int level, const TCHAR *format, ...)
{
	TCHAR buffer[4096];
	va_list vp;
	va_start(vp, format);
	vswprintf(buffer, 4096, format, vp);
	va_end(vp);

	logfile << buffer;
}