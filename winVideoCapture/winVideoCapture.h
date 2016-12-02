#pragma once

#include "resource.h"
#include <list>
#include "IVideoCapture.h"
#include "VideoCallback.h"
#include "IVPP.h"
#include "IVideoCodec.h"
#include "IRender.h"

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
	OPEN_DEVICE_PARAM captureArgs;
	IVideoCapture *capture;
	CVideoCallback *callBack;

	/* codec */
	ICodecFactory *encFactory;
	ICodec *codec;
	ENCODECCFG encoderArgs;

	/* render */
	IRenderFactory *renderFactory;
	IRender *render;

	CRITICAL_SECTION listLock;
	
	tagProgramContext(){
		ZeroMemory(this, sizeof(struct tagProgramContext));
	}
}THIS_CONTEXT, *PTHIS_CONTEXT;

BOOL StartCaptureWork(THIS_CONTEXT *ctx);
BOOL StopCaptureWork(THIS_CONTEXT *ctx);
BOOL SetupEncodeWork(THIS_CONTEXT *ctx);
BOOL StopEncodeWork(THIS_CONTEXT *ctx);
DWORD WINAPI CaptureThread(LPVOID args);
DWORD WINAPI RenderThread(LPVOID args);
BOOL CreateWorkThread(THIS_CONTEXT *ctx);
BOOL StartRenderWork(THIS_CONTEXT *ctx);
BOOL StopRenderWork(THIS_CONTEXT *ctx);
