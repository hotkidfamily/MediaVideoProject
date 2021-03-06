#include "StdAfx.h"
#include "logger.h"

static PLOG_CALLBACK_FUNC gLogCallback = nullptr;
#ifdef _DEBUG
static int level = Debug;
#else
static int level = Info;
#endif


void SetLogCallback( PLOG_CALLBACK_FUNC pCallbackFunction )
{
	gLogCallback = pCallbackFunction;
}

void logger( int lv, const char *fmt, ... )
{
	va_list vl;

	if (lv < level){
		return;
	}

	va_start(vl, fmt);
	if (gLogCallback){
		gLogCallback(lv, fmt, vl);
	}else{
		char loginfo[4096] = {'\0'};
		_vsnprintf_s(loginfo, sizeof(loginfo), _countof(loginfo)-1, fmt, vl);
		OutputDebugStringA(loginfo);
	}
	va_end(vl);
};
