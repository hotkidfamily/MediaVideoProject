#include "stdafx.h"
#include "FFmpegProcess.h"
#include "FFmpegWrapper.h"

FFmpegProcess::FFmpegProcess()
	: mScaleCtx(nullptr)
{
	FFmpegWrapper::InitModules();
}


FFmpegProcess::~FFmpegProcess()
{
	FFmpegWrapper::UnInitModules();
}

BOOL FFmpegProcess::InitContext(IVPPPARAMETER params)
{
	if ((mParams == params) && mScaleCtx){
		return TRUE;
	}

	mParams = params;
		
	mScaleCtx = FFmpegWrapper::sws_getContext(mParams.sourceWidth(), mParams.sourceHeight(), mParams.sourceFormat(),
		mParams.destWidth(), mParams.destHeight(), mParams.destFormat(), mParams.flag(), 0, 0, 0);

	return mScaleCtx != nullptr;
}

BOOL FFmpegProcess::DeinitContext()
{
	if (mScaleCtx)
		FFmpegWrapper::sws_freeContext((SwsContext*)mScaleCtx);

	mScaleCtx = nullptr;

	return TRUE;
}

BOOL FFmpegProcess::ProcessFrame(const CSampleBuffer *srcPic, CSampleBuffer *outPic)
{
	uint8_t **sBuf = nullptr;
	int32_t *sStride = nullptr;
	uint8_t **dBuf = nullptr;
	int32_t *dStride = nullptr;

	sBuf = srcPic->GetPlanarPtr();
	sStride = srcPic->GetStride();
	dBuf = outPic->GetPlanarPtr();
	dStride = outPic->GetStride();

	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.sourceHeight(), dBuf, dStride);

	return true;
}
