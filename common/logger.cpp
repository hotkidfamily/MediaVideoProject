#include "StdAfx.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

static PLOG_CALLBACK_FUNC gLogCallback = nullptr;

void SetLogCallback( PLOG_CALLBACK_FUNC pCallbackFunction )
{
	gLogCallback = pCallbackFunction;
}

void internel_log( int level, const char *fmt, ... )
{
	va_list vl;
	va_start(vl, fmt);
	if (gLogCallback){
		gLogCallback(level, fmt, vl);
	}else{
		char loginfo[4096] = {'\0'};
		_vsnprintf_s(loginfo, sizeof(loginfo), _countof(loginfo)-1, fmt, vl);
		OutputDebugStringA(loginfo);
	}
	va_end(vl);
};
