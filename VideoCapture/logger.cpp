#include "StdAfx.h"
#include "IVideoCapture.h"
#include "logger.h"

static PVIDEO_CAPTURE_LOG_CALLBACK_FUNC gLogCallback = NULL;

void SetLogCallback( PVIDEO_CAPTURE_LOG_CALLBACK_FUNC pCallbackFunction )
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
		char loginfo[4096];
		_vsnprintf_s(loginfo, sizeof(loginfo), _countof(loginfo)-1, fmt, vl);
		OutputDebugStringA(loginfo);
	}
	va_end(vl);
};
