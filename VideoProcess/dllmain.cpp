// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "IVPP.h"
#include "VideoProcess.h"

static long refCount = 1;
static VPPFactory *factory = NULL;
static CRITICAL_SECTION opera;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		InitializeCriticalSection(&opera);
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&opera);
		break;
	}
	return TRUE;
}

VPPFactory *GetVPPFactoryObj()
{
	IVPP *vpp = NULL;

	EnterCriticalSection(&opera);
	++refCount;
	if (!factory){
		factory = new VPPFactory;
	}

	LeaveCriticalSection(&opera);

	return factory;
}

void ReleaseVPPFctoryObj(VPPFactory *ctx)
{
	EnterCriticalSection(&opera);
	--refCount;
	if (refCount <= 0){
		delete ctx;
		ctx = NULL;
	}
	LeaveCriticalSection(&opera);
}