#pragma once

#include "resource.h"
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
	OPEN_DEVICE_PARAM captureArgs;
	IVideoCapture *capturer;
	CVideoCallback *callBack;
	BOOL bRuning;
	HANDLE hWorkThread;
	DWORD dwThreadId;

	/* codec */
	ICodecFactory *encFactory;
	ICodec *codec;
	ENCODECCFG encoderArgs;

	/* render */
	IRenderFactory *renderFactory;
	IRender *render;

	tagProgramContext(){
		ZeroMemory(this, sizeof(struct tagProgramContext));
	}
}THIS_CONTEXT, *PTHIS_CONTEXT;

BOOL StartCaptureWork(THIS_CONTEXT *ctx);
BOOL StopCaptureWork(THIS_CONTEXT *ctx);
BOOL SetupEncodeWork(THIS_CONTEXT *ctx);
BOOL StopEncodeWork(THIS_CONTEXT *ctx);
DWORD WINAPI EncoderThread(LPVOID args);
BOOL CreateWorkThread(THIS_CONTEXT *ctx);
BOOL StartRenderWork(THIS_CONTEXT *ctx);
BOOL StopRenderWork(THIS_CONTEXT *ctx);
