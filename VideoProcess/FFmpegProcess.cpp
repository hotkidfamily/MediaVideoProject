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
		
	mScaleCtx = FFmpegWrapper::sws_getContext(mParams.inFrame.Width(), mParams.inFrame.Height(), mParams.inFrame.PixelFormatInFFmpeg,
		mParams.outFrame.Width(), mParams.outFrame.Height(), mParams.outFrame.PixelFormatInFFmpeg, mParams.flags, 0, 0, 0);

	if (!mScaleCtx){
		return false;
	}

	int ret = FFmpegWrapper::sws_setColorspaceDetails(mScaleCtx, FFmpegWrapper::sws_getCoefficients(mParams.inFrame.colorSpace), mParams.inFrame.colorRange,
		FFmpegWrapper::sws_getCoefficients(mParams.outFrame.colorSpace), mParams.outFrame.colorSpace, 0, 0, 0);

	return ret == 0;
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
	int64_t ptss = 0 ;
	int64_t ptse = 0 ;

	sBuf = srcPic->GetPlanarPtr();
	sStride = srcPic->GetStride();
	dBuf = outPic->GetPlanarPtr();
	dStride = outPic->GetStride();

	if (srcPic->GetPixelFormat() == PIXEL_FORMAT_RGB24 || srcPic->GetPixelFormat() == PIXEL_FORMAT_RGB32){
		sBuf[0] = sBuf[0] + sStride[0] + srcPic->GetLineSize() * (srcPic->GetHeight() - 1);
		sStride[0] = -sStride[0];
	}

	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.inFrame.Height(), dBuf, dStride);

	srcPic->GetPts(ptss, ptse);
	outPic->SetPts(ptss, ptse);

	return true;
}
