#include "stdafx.h"
#include "FFmpegProcess.h"
#include "FFmpegWrapper.h"

FFmpegProcess::FFmpegProcess()
	: mScaleCtx(NULL)
{
}


FFmpegProcess::~FFmpegProcess()
{
}

BOOL FFmpegProcess::InitiaContext(FFmpegColorConvertParams params)
{
	if ((mParams == params) && mScaleCtx){
		return TRUE;
	}
		
	mScaleCtx = FFmpegWrapper::sws_getContext(params.sourceWidth(), params.sourceHeight(), params.spFormat,
		params.destWidth(), params.destHeight(), params.destFormat(), params.flag(), 0, 0, 0);

	mParams = params;

	return mScaleCtx != NULL;
}

BOOL FFmpegProcess::DeinitContext()
{
	if (mScaleCtx)
		FFmpegWrapper::sws_freeContext((SwsContext*)mScaleCtx);

	mScaleCtx = NULL;

	return TRUE;
}

BOOL FFmpegProcess::ProcessFrame(uint8_t* sBuf[4], int sStride[4], uint8_t* dBuf[4], int dStride[4])
{
	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.sourceHeight(), dBuf, dStride);

	return true;
}
