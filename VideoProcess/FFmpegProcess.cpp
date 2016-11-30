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
	uint8_t *sBuf[4] = { 0 };
	int32_t sStride[4] = { 0 };
	uint8_t *dBuf[4] = { 0 };
	int32_t dStride[4] = { 0 };

	sBuf[0] = srcPic->GetDataPtr();
	sStride[0] = srcPic->GetLineSize();
	dBuf[0] = outPic->GetDataPtr();
	dStride[0] = outPic->GetLineSize();

	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.sourceHeight(), dBuf, dStride);

	return true;
}
