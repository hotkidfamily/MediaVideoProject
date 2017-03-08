#pragma once

#include "IVPP.h"
#include "FFmpegWrapper.h"
#include "common\PixelFormat.h"
#include "common\utils.h"
#include "VppUtils.h"

struct FFmpegScaleParams
{
	FFmpegScaleParams(){}

	FFmpegScaleParams(IVPPPARAMETER &params){
		vppParams = params;
		inFramePixFmt = (AVPixelFormat)GetPixFmtByFourCC(params.inDesc.pixelFormat);
		outFramePixFmt = (AVPixelFormat)GetPixFmtByFourCC(params.outDesc.pixelFormat);
	}

	FFmpegScaleParams &operator = (IVPPPARAMETER &params){
		vppParams = params;
		inFramePixFmt = (AVPixelFormat)GetPixFmtByFourCC(params.inDesc.pixelFormat);
		outFramePixFmt = (AVPixelFormat)GetPixFmtByFourCC(params.outDesc.pixelFormat);
		return *this;
	}

	bool operator == (IVPPPARAMETER &params){
		return (vppParams == params);
	}

	IVPPPARAMETER vppParams;
	AVPixelFormat inFramePixFmt;
	AVPixelFormat outFramePixFmt;
};

class FFmpegProcess : public IVPP
{
public:
	FFmpegProcess();
	~FFmpegProcess();

	virtual BOOL InitContext(IVPPPARAMETER params);

	virtual BOOL DeinitContext();

	virtual BOOL ProcessFrame(const VideoSampleBuffer *srcPic, VideoSampleBuffer *outPic);

private:
	SwsContext *mScaleCtx;
	FFmpegScaleParams mParams;
};

