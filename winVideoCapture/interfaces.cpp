
#include "stdafx.h"
#include "winVideoCapture.h"
#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>

void log(const char *format, ...)
{
	char str[256];
	va_list vl;
	va_start(vl, format);

	_vsnprintf_s(str, 255, format, vl);

	va_end(vl);

	OutputDebugStringA(str);
}

std::list<VideoSampleBuffer*> captureList;

BOOL StartCamCaptureWork(THIS_CONTEXT *ctx)
{
	HRESULT hr = E_FAIL;

	if (!ctx->bRuning){
		if (!ctx->callBack)
			ctx->callBack = new CVideoCallback;
		assert(ctx->callBack);
		captureList.clear();

		ctx->capture->RegisterCallback(ctx->callBack);
		ctx->captureCfg.parentWindow = ctx->hMainWnd;
		ctx->captureCfg.fps = { 30, 1 };
		ctx->captureCfg.width = 1280;
		ctx->captureCfg.height = 720;
		hr = ctx->capture->StartCaptureWithParam(ctx->captureCfg);
		if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)){
			MessageBox(ctx->hMainWnd, TEXT("Device have been occured."), TEXT("ERROR"), MB_OK);
		}
		if (hr == S_OK){
			ctx->bRuning = TRUE;
		}
	}

	return hr == S_OK;
}

BOOL StartFileCaptureWork(THIS_CONTEXT *ctx)
{
	HRESULT hr = E_FAIL;

	if (!ctx->bRuning){
		captureList.clear();
		ctx->capture = GetVideoCaptureObj();
		assert(ctx->capture);
		ctx->captureCfg.parentWindow = ctx->hMainWnd;
		hr = ctx->capture->StartCaptureWithParam(ctx->captureCfg);
		ctx->bRuning = TRUE;
	}

	return hr == S_OK;
}

BOOL StopCaptureWork(THIS_CONTEXT *ctx)
{
	captureList.clear();
	if (ctx->capture){
		ctx->capture->StopCapture();
		ctx->capture->UnRegisterCallback();
	}

	SAFE_DELETE(ctx->callBack);

	return TRUE;
}

BOOL StopEncodeWork(THIS_CONTEXT *ctx)
{
	if (ctx->codec){
		ctx->codec->close();
	}

	if (ctx->encFactory){
		ctx->encFactory->DestoryCodecObj(ctx->codec);
		ctx->codec = nullptr;
		ReleaseCodecFactoryOBj(ctx->encFactory);
		ctx->encFactory = nullptr;
	}

	ctx->encoderCfg.cfgStr.clear();

	return TRUE;
}

BOOL SetupEncodeWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = TRUE;

	if (!GetCodecFactoryOBj(ctx->encFactory)){
		bRet = FALSE;
	} else{
		if (!ctx->encFactory->CreateCodecObj(ctx->codec)){
			bRet = FALSE;
		}
	}

	if (bRet)
	{
		ctx->encoderCfg.fps = ctx->captureCfg.fps;
		ctx->encoderCfg.width = ctx->captureCfg.width;
		ctx->encoderCfg.height = ctx->captureCfg.height;
		ctx->encoderCfg.avgBitrateInKb = 2000;
		ctx->encoderCfg.minBitrateInKb = 2000;
		ctx->encoderCfg.maxBitrateInKb = 2000;
		ctx->encoderCfg.pixelFormat = ctx->captureCfg.pixelFormat;
		ctx->encoderCfg.cfgStr.append(TEXT("keyint=75:min-keyint=75:scenecut=0:bframes=2:b-adapt=0:b-pyramid=none:threads=0:sliced-threads=0:ref=2:subme=2:me=hex:analyse=i4x4,i8x8,p8x8,p4x4,b8x8:direct=spatial:weightp=1:weightb=1:8x8dct=1:cabac=1:deblock=0,0:psy=0:trellis=0:aq-mode=1:rc-lookahead=4:sync-lookahead=0:mbtree=0:"));

		ctx->codec->setConfig(ctx->encoderCfg);
		bRet = ctx->codec->open();
	}

	if (!bRet){
		StopEncodeWork(ctx);
	}
	return FALSE;
}

DWORD WINAPI CaptureThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;

	while (ctx->bRuning){
		VideoSampleBuffer *frame = nullptr;
		if (ctx->capture->GetFrame(frame)){
			EnterCriticalSection(&ctx->listLock);
			captureList.push_back(frame);
			LeaveCriticalSection(&ctx->listLock);
		}

		Sleep(1);
	}

	return TRUE;
}

DWORD WINAPI RenderThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;
	std::ofstream encodeFile;
	if (ctx->bEnableCodec){
		encodeFile.open(TEXT("D:\\capture.h264"), std::ios::binary);
	}

	while (ctx->bRuning){
		VideoSampleBuffer *frame = nullptr;
		CPackageBuffer *packet = nullptr;

		if (!captureList.empty()){
			EnterCriticalSection(&ctx->listLock);
			frame = captureList.front();
			captureList.pop_front();
			LeaveCriticalSection(&ctx->listLock);
			ctx->render->PushFrame(frame);
			if (ctx->bEnableCodec){
				ctx->codec->addFrame(*frame);
				if (ctx->codec->getPackage(packet)){
					if (packet->isIDRFrame())
						encodeFile.write((const char*)(packet->ExtraData()), packet->ExtraDataSize());
					encodeFile.write((const char*)packet->Data(), packet->DataSize());
					ctx->codec->releasePackage(packet);
				}
			}			
			ctx->capture->ReleaseFrame(frame);
		}

		Sleep(1);
	}

	return TRUE;
}

BOOL CreateWorkThread(THIS_CONTEXT *ctx)
{
	ctx->hCaptureThread = CreateThread(nullptr, 0, CaptureThread, ctx, 0, &(ctx->dwCaptureThreadID));
	ctx->hRenderThread = CreateThread(nullptr, 0, RenderThread, ctx, 0, &(ctx->dwRenderThreadID));
	return TRUE;
}

BOOL DestoryWorkThread(THIS_CONTEXT *ctx)
{
	ctx->bRuning = 0;
	if (ctx->hCaptureThread){
		if (WAIT_OBJECT_0 != WaitForSingleObject(ctx->hCaptureThread, INFINITE)){
			TerminateThread(ctx->hCaptureThread, -1);
			ctx->hCaptureThread = NULL;
		}
	}
	log("capture thread end.\n");

	if (ctx->hRenderThread){
		if (WAIT_OBJECT_0 != WaitForSingleObject(ctx->hRenderThread, INFINITE)){
			TerminateThread(ctx->hRenderThread, -1);
			ctx->hRenderThread = NULL;
		}
	}
	log("render thread end.\n");

	return TRUE;
}

BOOL StopRenderWork(THIS_CONTEXT *ctx)
{
	if (ctx){
		if (ctx->render){
			ctx->render->DeinitRender();
			ctx->renderFactory->DestoryRenderObj(ctx->render);
			ctx->render = NULL;
		}

		ReleaseRenderFactoryObj(ctx->renderFactory);
		ctx->renderFactory = NULL;
	}

	return TRUE;
}

BOOL StartRenderWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = TRUE;
	bRet = GetRenderFactoryObj(ctx->renderFactory);
	if (bRet){
		bRet = ctx->renderFactory->CreateRenderObj(ctx->render);
	}

	ctx->renderCfg.bWaitVSync = TRUE;
	ctx->renderCfg.hWnd = ctx->hMainWnd;
	ctx->renderCfg.width = ctx->captureCfg.width;
	ctx->renderCfg.height = ctx->captureCfg.height;
	ctx->renderCfg.pixelFormat = ctx->captureCfg.pixelFormat;
	ctx->renderCfg.fps.num = ctx->captureCfg.fps.num;
	ctx->renderCfg.fps.den = ctx->captureCfg.fps.den;

	if(bRet){
		bRet = ctx->render->InitRender(ctx->renderCfg);
	}

	if(!bRet){
		StopRenderWork(ctx);
	}

	return TRUE;
}

void ResizeWindow(THIS_CONTEXT *ctx)
{
	RECT rect = { 0 };
	RECT rectClient = { 0 };
	int width = 0, height = 0;
	GetWindowRect(ctx->hMainWnd, &rect);
	GetClientRect(ctx->hMainWnd, &rectClient);
	int32_t mwidth = rect.right - rect.left - rectClient.right;
	int32_t mheight = rect.bottom - rect.top - rectClient.bottom;
	int swidth = GetSystemMetrics(SM_CXSCREEN);
	int sheight = GetSystemMetrics(SM_CYSCREEN);

	width = ctx->captureCfg.width > swidth? swidth:ctx->captureCfg.width;
	height = ctx->captureCfg.height > sheight ? sheight : ctx->captureCfg.height;
	
	MoveWindow(ctx->hMainWnd, rect.left, rect.top, width + mwidth, height + mheight, TRUE);
}

void StopStream(THIS_CONTEXT *ctx) 
{
	DestoryWorkThread(ctx);
	StopCaptureWork(ctx);
	StopRenderWork(ctx);
	if (ctx->bEnableCodec){
		StopEncodeWork(ctx);
	}
	ctx->bEnableCodec = FALSE;
}

BOOL SetupStream(THIS_CONTEXT *ctx, BOOL bCam)
{
	BOOL bSuccess = FALSE;
	if (bCam){
		bSuccess = StartCamCaptureWork(ctx);
	} else{
		bSuccess = StartFileCaptureWork(ctx);
	}
	if (bSuccess){
		ResizeWindow(ctx);		
		StartRenderWork(ctx);
		if (ctx->bEnableCodec){
			SetupEncodeWork(ctx);
		}
		CreateWorkThread(ctx);
	}

	return bSuccess;
}
