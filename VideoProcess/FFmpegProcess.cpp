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
		
	mScaleCtx = FFmpegWrapper::sws_getContext(mParams.vppParams.inDesc.Width(), mParams.vppParams.inDesc.Height(), mParams.inFramePixFmt,
		mParams.vppParams.outDesc.Width(), mParams.vppParams.outDesc.Height(), mParams.outFramePixFmt, mParams.vppParams.flags, 0, 0, 0);

	if (!mScaleCtx){
		return false;
	}

 	int ret = FFmpegWrapper::sws_setColorspaceDetails(mScaleCtx, FFmpegWrapper::sws_getCoefficients(mParams.vppParams.inDesc.colorSpace), mParams.vppParams.inDesc.colorRange,
 		FFmpegWrapper::sws_getCoefficients(mParams.vppParams.outDesc.colorSpace), mParams.vppParams.outDesc.colorRange, 0, 1<<16, 1<<16);

	return ret == 0;
}

BOOL FFmpegProcess::DeinitContext()
{
	if (mScaleCtx)
		FFmpegWrapper::sws_freeContext((SwsContext*)mScaleCtx);

	mScaleCtx = nullptr;

	return TRUE;
}

BOOL FFmpegProcess::ProcessFrame(const VideoSampleBuffer *srcPic, VideoSampleBuffer *outPic)
{
	uint8_t **sBuf = nullptr;
	int32_t *sStride = nullptr;
	uint8_t **dBuf = nullptr;
	int32_t *dStride = nullptr;
	int32_t *lineSize = nullptr;

	sBuf = (uint8_t **)srcPic->planarPtr;
	sStride = (int32_t*)srcPic->planarStride;
	dBuf = outPic->planarPtr;
	dStride = outPic->planarStride;
	lineSize = (int32_t*)srcPic->planarStride;

	if (srcPic->pixelFormatInFourCC == PIXEL_FORMAT_RGB24 || srcPic->pixelFormatInFourCC == PIXEL_FORMAT_RGB32){
		sBuf[0] = sBuf[0] + sStride[0] + lineSize[0] * (srcPic->height - 1);
		sStride[0] = -sStride[0];
	}

	int oheight = FFmpegWrapper::sws_scale(mScaleCtx, sBuf, sStride, 0, mParams.vppParams.inDesc.Height(), dBuf, dStride);

	outPic->ptsStart = srcPic->ptsStart;
	outPic->ptsEnd = srcPic->ptsEnd;
	outPic->width = srcPic->width;
	outPic->height = srcPic->height;

	outPic->ptsStart = srcPic->ptsStart;
	outPic->ptsEnd = srcPic->ptsEnd;
	outPic->frameStartIdx = srcPic->frameStartIdx;
	outPic->frameEndIdx = srcPic->frameEndIdx;

	return true;
}
