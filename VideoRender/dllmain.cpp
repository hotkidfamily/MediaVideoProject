#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "VideoRender.h"

static PVIDEO_RENDER_LOG_CALLBACK_FUNC gLogCallback = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void SetLogCallback(PVIDEO_RENDER_LOG_CALLBACK_FUNC pCallbackFunction)
{
	gLogCallback = pCallbackFunction;
}

void internel_log(int level, const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	if (gLogCallback){
		gLogCallback(level, fmt, vl);
	}
	else{
		char loginfo[4096] = { '\0' };
		_vsnprintf_s(loginfo, sizeof(loginfo), _countof(loginfo) - 1, fmt, vl);
		OutputDebugStringA(loginfo);
	}
	va_end(vl);
};

VIDEORENDER_API IRender *GetRenderObj()
{
	IRender *pRender = NULL;
	IRenderDelegate *pRenderDel = new IRenderDelegate;
	if (!pRender){
		pRender = static_cast<IRender*>(pRenderDel);
	}

	return pRender;
}

VIDEORENDER_API void ReleaseRenderObj(IRender * pRender)
{
	if (pRender){
		IRenderDelegate *pD = static_cast<IRenderDelegate*>(pRender);
		delete pD;
	}
}
