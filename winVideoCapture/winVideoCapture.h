#pragma once

#include "resource.h"
#include "VideoCallback.h"

#define MAX_LOADSTRING 100

typedef struct tagProgramContext{
	HINSTANCE hInstance;
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];

	HWND hMainWnd;
	HWND hDashboardWnd;
	HWND hMediaInfoWnd;
	int32_t totalCaptureDevices;

	BOOL bRuning;
	HANDLE hCaptureThread;
	DWORD dwCaptureThreadID;
	HANDLE hRenderThread;
	DWORD dwRenderThreadID;

	/* capture */
	CAPTURECONFIG captureCfg;
	IVideoCapture *capture;
	CVideoCallback *callBack;

	/* codec */
	ICodecFactory *encFactory;
	ICodec *codec;
	ENCODECCFG encoderCfg;

	/* render */
	IRenderFactory *renderFactory;
	IRender *render;
	RENDERCONFIG renderCfg;

	CRITICAL_SECTION listLock;
	
	tagProgramContext(){
		ZeroMemory(this, sizeof(struct tagProgramContext));
	}
}THIS_CONTEXT, *PTHIS_CONTEXT;

void ResizeWindow(THIS_CONTEXT *ctx);
void StopStream(THIS_CONTEXT *ctx);
BOOL SetupStream(THIS_CONTEXT *ctx, BOOL bCam);
