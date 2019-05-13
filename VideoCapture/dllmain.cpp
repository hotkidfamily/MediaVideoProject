// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "IVideoCapture.h"
#include "VideoCapture.h"

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

IVideoCapture *GetVideoCaptureObj()
{
	CoInitialize(nullptr);

	IVideoCaptureDelegate *pCapture = new IVideoCaptureDelegate;
	if (pCapture){
		pCapture->InitContext();
		return static_cast<IVideoCapture*>(pCapture);
	}else{
		return nullptr;
	}
}

void ReleaseVideoCaptureObj(IVideoCapture * pCapture)
{
	IVideoCaptureDelegate *pCap = static_cast<IVideoCaptureDelegate*>(pCapture);
	if (pCap){
		pCap->UnInitContext();
		delete pCap;
		pCap = NULL;
	}
	
	CoUninitialize();
}
