// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "IVPP.h"
#include "VideoProcess.h"

static long refCount = 1;
static IVPPFactoryImpl *factory = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BOOL GetVPPFactoryObj(IVPPFactory* &factoryPtr)
{
	InterlockedAdd(&refCount, 1);
	if (!factory){
		factory = new IVPPFactoryImpl;
	}

	factoryPtr = factory;

	return factoryPtr != NULL;
}

BOOL ReleaseVPPFctoryObj(IVPPFactory *ctx)
{
	BOOL bRet = FALSE;

	if (ctx == factory){
		if (!InterlockedDecrement(&refCount)){
			delete factory;
			factory = NULL;
			ctx = NULL;
		}
		bRet = TRUE;
	}
	
	return bRet;
}