#pragma once

#include "IVPP.h"
#include "FFmpegWrapper.h"
#include "PixelFormat.h"
#include "VppUtils.h"

typedef struct tagFrameDescInFFmpeg : public FRAMECOLORDESC{

	AVPixelFormat PixelFormatInFFmpeg;

	tagFrameDescInFFmpeg& operator = (FRAMECOLORDESC &desc){
		width = desc.width;
		height = desc.height;
		colorSpace = desc.colorSpace;
		colorRange = desc.colorRange;
		pixelFormat = desc.pixelFormat;
		PixelFormatInFFmpeg = GetPixFmtByFourCC(desc.pixelFormat);
		return *this;
	}

	bool operator == (FRAMECOLORDESC &desc){
		return ((FRAMECOLORDESC)*this == desc);
	}
}FrameDescInFFmpeg;

struct FFmpegScaleParams
{
	FFmpegScaleParams(){}

	FFmpegScaleParams(IVPPPARAMETER &params){
		inFrame = params.inDesc;
		outFrame = params.outDesc;
		flags = params.flags;
	}

	bool operator == (IVPPPARAMETER &params){
		return (params.inDesc == inFrame)
			&& (params.outDesc == outFrame)
			&& (flags == params.flags);
	}

	FrameDescInFFmpeg inFrame;
	FrameDescInFFmpeg outFrame;
	int32_t flags;
};

class FFmpegProcess : public IVPP
{
public:
	FFmpegProcess();
	~FFmpegProcess();

	virtual BOOL InitContext(IVPPPARAMETER params);

	virtual BOOL DeinitContext();

	virtual BOOL ProcessFrame(const CSampleBuffer *srcPic, CSampleBuffer *outPic);

private:
	SwsContext *mScaleCtx;
	FFmpegScaleParams mParams;
};

