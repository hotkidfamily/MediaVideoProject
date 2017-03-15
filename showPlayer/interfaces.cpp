
#include "stdafx.h"
#include "showPlayer.h"
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

std::list<VideoSampleBuffer*> videoCaptureList;
std::list<AudioSampleBuffer*> AudioCaptureList;

BOOL StartCamCaptureWork(THIS_CONTEXT *ctx)
{
	HRESULT hr = E_FAIL;

	if (!ctx->bRuning){
		if (!ctx->callBack)
			ctx->callBack = new CVideoCallback;
		assert(ctx->callBack);
		videoCaptureList.clear();

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
		videoCaptureList.clear();
		AudioCaptureList.clear();
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
	videoCaptureList.clear();
	if (ctx->capture){
		ctx->capture->StopCapture();
		ctx->capture->UnRegisterCallback();
	}

	SAFE_DELETE(ctx->callBack);

	return TRUE;
}

BOOL DestroyEncodeWork(THIS_CONTEXT *ctx)
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
		DestroyEncodeWork(ctx);
	}
	return bRet;
}

DWORD WINAPI CaptureThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;

	while (ctx->bRuning){
		VideoSampleBuffer *frame = nullptr;
		AudioSampleBuffer *aframe = nullptr;
		if (ctx->capture->GetFrame(frame)){
			EnterCriticalSection(&ctx->listLock);
			videoCaptureList.push_back(frame);
			LeaveCriticalSection(&ctx->listLock);
		}

		if (ctx->capture->GetAudioFrame(aframe)){
			EnterCriticalSection(&ctx->listLock);
			AudioCaptureList.push_back(aframe);
			LeaveCriticalSection(&ctx->listLock);
		}

		Sleep(1);
	}

	return TRUE;
}

DWORD WINAPI VideoRenderThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;
	std::ofstream encodeFile;
	if (ctx->bEnableCodec){
		encodeFile.open(TEXT("D:\\capture.h264"), std::ios::binary);
	}

	while (ctx->bRuning){
		VideoSampleBuffer *frame = nullptr;
		CPackageBuffer *packet = nullptr;

		if (!videoCaptureList.empty()){
			EnterCriticalSection(&ctx->listLock);
			frame = videoCaptureList.front();
			videoCaptureList.pop_front();
			LeaveCriticalSection(&ctx->listLock);
			ctx->videoRender->PushFrame(frame);
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

DWORD WINAPI AudioRenderThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;
	std::ofstream encodeFile;

	while (ctx->bRuning){
		AudioSampleBuffer *frame = nullptr;

		if (!AudioCaptureList.empty()){
			EnterCriticalSection(&ctx->listLock);
			frame = AudioCaptureList.front();
			AudioCaptureList.pop_front();
			LeaveCriticalSection(&ctx->listLock);
			if(ctx->bAudioRender)
				ctx->audioRender->PushFrame(frame);

			ctx->capture->ReleaseAudioFrame(frame);
		}

		Sleep(1);
	}

	return TRUE;
}

BOOL CreateWorkThread(THIS_CONTEXT *ctx)
{
	ctx->hCaptureThread = CreateThread(nullptr, 0, CaptureThread, ctx, 0, &(ctx->dwCaptureThreadID));

	if (!ctx->captureCfg.bNoVideo)
		ctx->hVideoRenderThread = CreateThread(nullptr, 0, VideoRenderThread, ctx, 0, &(ctx->dwVideoRenderThreadID));

	if (!ctx->captureCfg.bNoAudio)
		ctx->hAudioRenderThread = CreateThread(nullptr, 0, AudioRenderThread, ctx, 0, &(ctx->dwAudioRenderThreadID));

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

	if (ctx->hAudioRenderThread){
		if (WAIT_OBJECT_0 != WaitForSingleObject(ctx->hAudioRenderThread, INFINITE)){
			TerminateThread(ctx->hAudioRenderThread, -1);
			ctx->hAudioRenderThread = NULL;
		}
	}
	log("audio render thread end.\n");

	if (ctx->hVideoRenderThread){
		if (WAIT_OBJECT_0 != WaitForSingleObject(ctx->hVideoRenderThread, INFINITE)){
			TerminateThread(ctx->hVideoRenderThread, -1);
			ctx->hVideoRenderThread = NULL;
		}
	}
	log("video render thread end.\n");

	return TRUE;
}

BOOL DestroyVideoRenderWork(THIS_CONTEXT *ctx)
{
	if (ctx){
		if (ctx->videoRender){
			ctx->videoRender->DeinitRender();
			ctx->videoRenderFactory->DestoryRenderObj(ctx->videoRender);
			ctx->videoRender = NULL;
		}

		ReleaseRenderFactoryObj(ctx->videoRenderFactory);
		ctx->videoRenderFactory = NULL;
	}

	return TRUE;
}

BOOL SetupVideoRenderWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = TRUE;
	bRet = GetRenderFactoryObj(ctx->videoRenderFactory);
	if (bRet){
		bRet = ctx->videoRenderFactory->CreateRenderObj(ctx->videoRender);
	}

	ctx->vRenderCfg.bWaitVSync = TRUE;
	ctx->vRenderCfg.hWnd = ctx->hMainWnd;
	ctx->vRenderCfg.width = ctx->captureCfg.width;
	ctx->vRenderCfg.height = ctx->captureCfg.height;
	ctx->vRenderCfg.pixelFormat = ctx->captureCfg.pixelFormat;
	ctx->vRenderCfg.fps.num = ctx->captureCfg.fps.num;
	ctx->vRenderCfg.fps.den = ctx->captureCfg.fps.den;

	if(bRet){
		bRet = ctx->videoRender->InitRender(ctx->vRenderCfg);
	}

	if(!bRet){
		DestroyVideoRenderWork(ctx);
	}

	return bRet;
}

BOOL DestroyAudioRenderWork(THIS_CONTEXT *ctx)
{
	if (ctx){
		if (ctx->audioRender){
			ctx->audioRender->DeinitRender();
			ctx->audioRenderFactory->DestoryAudioRenderObj(ctx->audioRender);
			ctx->audioRender = NULL;
		}

		ReleaseAudioRenderFactoryObj(ctx->audioRenderFactory);
		ctx->audioRenderFactory = NULL;
	}

	return TRUE;
}

BOOL SetupAudioRenderWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = TRUE;
	bRet = GetAudioRenderFactoryObj(ctx->audioRenderFactory);
	if (bRet){
		bRet = ctx->audioRenderFactory->CreateAudioRenderObj(ctx->audioRender);
	}

	ctx->aRenderCfg.wavFormat = TRUE;
	ctx->aRenderCfg.hWnd = ctx->hMainWnd;
	ctx->aRenderCfg.channels = ctx->captureCfg.width;
	ctx->aRenderCfg.bitsPerSample = ctx->captureCfg.height;
	ctx->aRenderCfg.sampleRate = ctx->captureCfg.pixelFormat;
	
	if (bRet){
		bRet = ctx->audioRender->InitRender(ctx->aRenderCfg);
	}

	if (!bRet){
		DestroyAudioRenderWork(ctx);
	}

	return bRet;
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
	DestroyVideoRenderWork(ctx);
	DestroyAudioRenderWork(ctx);
	if (ctx->bEnableCodec){
		DestroyEncodeWork(ctx);
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
		if (!ctx->captureCfg.bNoVideo){
			bSuccess = SetupVideoRenderWork(ctx);
			if (!bSuccess){
				ctx->captureCfg.bNoVideo = 1;
			}
		}

		if (!ctx->captureCfg.bNoAudio){
			bSuccess = SetupAudioRenderWork(ctx);
			if (!bSuccess){
				ctx->bAudioRender = 0;
			}
		}

		if (ctx->bEnableCodec  && !ctx->captureCfg.bNoVideo){
			SetupEncodeWork(ctx);
		}
		CreateWorkThread(ctx);
	}

	return bSuccess;
}
