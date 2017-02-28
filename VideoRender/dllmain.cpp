#include "stdafx.h"
#include "VideoRender.h"

static long refCount = 1;
static IRenderFactoryImpl *factory = nullptr;

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

BOOL GetRenderFactoryObj(IRenderFactory *&factoryPtr)
{
	InterlockedAdd(&refCount, 1);
	if (!factory){
		factory = new IRenderFactoryImpl;
	}

	factoryPtr = factory;

	return factoryPtr != nullptr;
}

BOOL ReleaseRenderFactoryObj(IRenderFactory* factoryPtr)
{
	BOOL bRet = FALSE;

	if (factoryPtr == factory){
		if (!InterlockedDecrement(&refCount)){
			delete factory;
			factory = nullptr;
			factoryPtr = nullptr;
		}
		bRet = TRUE;
	}

	return bRet;
}
