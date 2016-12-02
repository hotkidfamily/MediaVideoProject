// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "IVideoCodec.h"
#include "VideoCodec.h"

static long refCount = 1;
static ICodecFactoryImpl *factory = nullptr;

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

BOOL GetCodecFactoryOBj(ICodecFactory* &factoryPtr)
{
	InterlockedAdd(&refCount, 1);
	if (factory){
		factory = new ICodecFactoryImpl;
	}

	factoryPtr = factory;

	return factoryPtr != nullptr;
}

BOOL ReleaseCodecFactoryOBj(ICodecFactory *factoryPtr)
{
	BOOL bRet = TRUE;
	if (factoryPtr == factory){
		if (!InterlockedDecrement(&refCount)){
			if (factory){
				delete factory;
				factory = nullptr;
			}
		}
	}

	return bRet;
}