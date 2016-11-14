#pragma once

#include "resource.h"
#include "IVideoCapture.h"

#include "VideoCallback.h"
#include "encoder_x264.h"

#define MAX_LOADSTRING 100

typedef struct tagProgramContext{
	HINSTANCE hInst;
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	HWND mainWnd;
	OPEN_DEVICE_PARAM captureArgs;
	IVideoCapture *pVideoCapture;
	CVideoCallback *callBack;
	BOOL bRuning;
	HANDLE hWorkThread;
	DWORD dwThreadId;
	ENCODEC_CFG encoderArgs;
	CLibx264 *encoder;
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
