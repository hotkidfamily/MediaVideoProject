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
	HANDLE hVideoRenderThread;
	DWORD dwVideoRenderThreadID;
	HANDLE hAudioRenderThread;
	DWORD dwAudioRenderThreadID;

	/* capture */
	CAPTURECONFIG captureCfg;
	IVideoCapture *capture;
	CVideoCallback *callBack;

	/* codec */
	BOOL bEnableCodec;
	ICodecFactory *encFactory;
	ICodec *codec;
	ENCODECCFG encoderCfg;

	/* render */
	BOOL bVideoRender;
	IRenderFactory *videoRenderFactory;
	IRender *videoRender;
	RENDERCONFIG vRenderCfg;

	// audio render
	BOOL bAudioRender;
	IAudioRenderFactory *audioRenderFactory;
	IAudioRender *audioRender;
	AudioRenderConfig aRenderCfg;

	CRITICAL_SECTION listLock;
	
	tagProgramContext(){
		ZeroMemory(this, sizeof(struct tagProgramContext));
	}
}THIS_CONTEXT, *PTHIS_CONTEXT;

void ResizeWindow(THIS_CONTEXT *ctx);
void StopStream(THIS_CONTEXT *ctx);
BOOL SetupStream(THIS_CONTEXT *ctx, BOOL bCam);
