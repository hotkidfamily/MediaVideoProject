
#include "stdafx.h"
#include "winVideoCapture.h"
#include <assert.h>
#include <fstream>
#include "dshowutil.h"

BOOL StartCaptureWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = FALSE;

	if (!ctx->bRuning){
		ctx->callBack = new CVideoCallback;
		assert(ctx->callBack);

		ctx->pVideoCapture->RegisterCallback(ctx->callBack);
		ctx->captureArgs.parentWindow = ctx->mainWnd;
		ctx->captureArgs.avgFrameIntervalInNs = FramesPerSecToRefTime(30);
		ctx->captureArgs.width = 1280;
		ctx->captureArgs.height = 720;
		bRet = ctx->pVideoCapture->StartCaptureWithParam(ctx->captureArgs);

		ctx->bRuning = TRUE;
	}else{
		bRet = TRUE;
	}

	return bRet;
}

BOOL StopCaptureWork(THIS_CONTEXT *ctx)
{
	ctx->bRuning = 0;
	WaitForSingleObject(ctx->hWorkThread, INFINITE);

	assert(ctx);
	ctx->pVideoCapture->StopCapture();
	ctx->pVideoCapture->UnRegisterCallback();

	SAFE_DELETE(ctx->callBack);

	return TRUE;
}

BOOL SetupEncodeWork(THIS_CONTEXT *ctx)
{
	ctx->encoder = new CLibx264;
	assert(ctx->encoder);
	ctx->encoderArgs.fps = 25;
	ctx->encoderArgs.width = ctx->captureArgs.width;
	ctx->encoderArgs.height = ctx->captureArgs.height;
	ctx->encoderArgs.avgBitrateInKb = 2000;
	ctx->encoderArgs.minBitrateInKb = 2000;
	ctx->encoderArgs.maxBitrateInKb = 2000;
	ctx->encoderArgs.cfgStr.append(TEXT("keyint=75:min-keyint=75:scenecut=0:bframes=2:b-adapt=0:b-pyramid=none:threads=1:sliced-threads=0:ref=2:subme=2:me=hex:analyse=i4x4,i8x8,p8x8,p4x4,b8x8:direct=spatial:weightp=1:weightb=1:8x8dct=1:cabac=1:deblock=0,0:psy=0:trellis=0:aq-mode=1:rc-lookahead=0:sync-lookahead=0:mbtree=0:"));

	ctx->encoder->setConfig(ctx->encoderArgs);
	return ctx->encoder->open();
}

BOOL StopEncodeWork(THIS_CONTEXT *ctx)
{
	SAFE_DELETE(ctx->encoder);
	return TRUE;
}

DWORD WINAPI EncoderThread(LPVOID args)
{
	THIS_CONTEXT * ctx = (THIS_CONTEXT *)args;
	std::ofstream encodeFile;
	encodeFile.open(TEXT("C:\\Users\\hotkid\\desktop\\capture.h264"), std::ios::binary);

	while (ctx->bRuning){
		CSampleBuffer* buffer = NULL;
		DwVideoPackage packet;
		if (ctx->callBack->GetFrame(buffer)){
			ctx->encoder->addFrame(*buffer);
			ctx->callBack->ReleaseFrame(buffer);
			ctx->encoder->getPackage(packet);
			if (packet.isIDRFrame())
				encodeFile.write((const char*)(packet.extraData), packet.extraDataSize);
			else
				encodeFile.write((const char*)packet.packageData, packet.packageDataSize);
		}

		Sleep(1);
	}

	return TRUE;
}

BOOL CreateWorkThread(THIS_CONTEXT *ctx)
{
	ctx->hWorkThread = CreateThread(NULL, 0, EncoderThread, ctx, 0, &(ctx->dwThreadId));
	return TRUE;
}