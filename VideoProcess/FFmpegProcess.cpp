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

BOOL FFmpegProcess::InitContext(IVPPPARAMETER params)
{
	if ((mParams == params) && mScaleCtx){
		return TRUE;
	}

	mParams = params;
		
	mScaleCtx = FFmpegWrapper::sws_getContext(mParams.sourceWidth(), mParams.sourceHeight(), mParams.sourceFormat(),
		mParams.destWidth(), mParams.destHeight(), mParams.destFormat(), mParams.flag(), 0, 0, 0);

	return mScaleCtx != NULL;
}

BOOL FFmpegProcess::DeinitContext()
{
	if (mScaleCtx)
		FFmpegWrapper::sws_freeContext((SwsContext*)mScaleCtx);

	mScaleCtx = NULL;

	return TRUE;
}

BOOL FFmpegProcess::ProcessFrame(const CSampleBuffer *srcPic, CSampleBuffer *outPic)
{
	uint8_t **sBuf = NULL;
	int32_t *sStride = NULL;
	uint8_t **dBuf = NULL;
	int32_t *dStride = NULL;

	sBuf = srcPic->GetPlanePtr();
	sStride = srcPic->GetStride();
	dBuf = outPic->GetPlanePtr();
	dStride = outPic->GetStride();

	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.sourceHeight(), dBuf, dStride);

	return true;
}
