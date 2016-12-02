
#include "stdafx.h"
#include "winVideoCapture.h"
#include <assert.h>
#include <fstream>

#pragma comment(lib, "VideoCodec.lib")
#pragma comment(lib, "VideoRender.lib")

std::list<CSampleBuffer*> captureList;

BOOL StartCaptureWork(THIS_CONTEXT *ctx)
{
	HRESULT hr = E_FAIL;

	if (!ctx->bRuning){
		if (!ctx->callBack)
			ctx->callBack = new CVideoCallback;
		assert(ctx->callBack);
		captureList.clear();

		ctx->capture->RegisterCallback(ctx->callBack);
		ctx->captureArgs.parentWindow = ctx->hMainWnd;
		ctx->captureArgs.fps = 30;
		ctx->captureArgs.width = 1280;
		ctx->captureArgs.height = 720;
		hr = ctx->capture->StartCaptureWithParam(ctx->captureArgs);
		if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)){
			MessageBox(ctx->hMainWnd, TEXT("Device have been occured."), TEXT("ERROR"), MB_OK);
		}
		if (hr == S_OK){
			ctx->bRuning = TRUE;
		}
	}

	return hr == S_OK;
}

BOOL StopCaptureWork(THIS_CONTEXT *ctx)
{
	if (ctx->bRuning){
		ctx->bRuning = 0;
		if (WAIT_OBJECT_0 != WaitForSingleObject(ctx->hCaptureThread, INFINITE)){
			assert(nullptr);
		}

		ctx->capture->StopCapture();
		ctx->capture->UnRegisterCallback();
	}

	SAFE_DELETE(ctx->callBack);

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
		ctx->encoderArgs.fps = (uint32_t)(ctx->captureArgs.fps);
		ctx->encoderArgs.width = ctx->captureArgs.width;
		ctx->encoderArgs.height = ctx->captureArgs.height;
		ctx->encoderArgs.avgBitrateInKb = 2000;
		ctx->encoderArgs.minBitrateInKb = 2000;
		ctx->encoderArgs.maxBitrateInKb = 2000;
		ctx->encoderArgs.pixelFormatInFourCC = ctx->captureArgs.pixelFormatInFourCC;
		ctx->encoderArgs.cfgStr.append(TEXT("keyint=75:min-keyint=75:scenecut=0:bframes=2:b-adapt=0:b-pyramid=none:threads=1:sliced-threads=0:ref=2:subme=2:me=hex:analyse=i4x4,i8x8,p8x8,p4x4,b8x8:direct=spatial:weightp=1:weightb=1:8x8dct=1:cabac=1:deblock=0,0:psy=0:trellis=0:aq-mode=1:rc-lookahead=0:sync-lookahead=0:mbtree=0:"));

		ctx->codec->setConfig(ctx->encoderArgs);
		bRet = ctx->codec->open();
	}

	if (!bRet){
		StopEncodeWork(ctx);
	}
	return FALSE;
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

	ctx->encoderArgs.cfgStr.clear();

	return TRUE;
}

DWORD WINAPI CaptureThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;

	while (ctx->bRuning){
		CSampleBuffer *frame = nullptr;
		if (ctx->capture->GetFrame(frame)){
			EnterCriticalSection(&ctx->listLock);
			captureList.push_back(frame);
			LeaveCriticalSection(&ctx->listLock);
			//ctx->capturer->ReleaseFrame(frame);
		}

		Sleep(20);
	}

	return TRUE;
}

DWORD WINAPI RenderThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;

	while (ctx->bRuning){
		CSampleBuffer *frame = nullptr;
		if (captureList.size()){
			EnterCriticalSection(&ctx->listLock);
			frame = captureList.front();
			ctx->render->PushFrame(frame);
			captureList.pop_front();
			LeaveCriticalSection(&ctx->listLock);
			
			ctx->capture->ReleaseFrame(frame);
		}
		
		Sleep(10);
	}

	return TRUE;
}

BOOL CreateWorkThread(THIS_CONTEXT *ctx)
{
	ctx->hCaptureThread = CreateThread(nullptr, 0, CaptureThread, ctx, 0, &(ctx->dwCaptureThreadID));
	ctx->hRenderThread = CreateThread(nullptr, 0, RenderThread, ctx, 0, &(ctx->dwRenderThreadID));
	return TRUE;
}

BOOL StartRenderWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = TRUE;
	bRet = GetRenderFactoryObj(ctx->renderFactory);
	if (bRet){
		bRet = ctx->renderFactory->CreateRenderObj(ctx->render);
	}

	if(bRet){
		bRet = ctx->render->InitRender(ctx->hMainWnd, ctx->captureArgs.width, ctx->captureArgs.height, ctx->captureArgs.pixelFormatInFourCC);
	}

	if(!bRet){
		StopRenderWork(ctx);
	}

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