#pragma once

#include "IVPP.h"
#include "FFmpegWrapper.h"
#include "PixelFormat.h"
#include "VppUtils.h"
#include <d3d9.h>

class FFmpegScaleParams : public IVPPPARAMETER
{
public:
	/**/
	inline int32_t sourceWidth() const { return abs(srcWidth); };
	inline int32_t sourceHeight() const { return abs(srcHeight); };
	inline AVPixelFormat sourceFormat() const { return srcPixelFormat; };
	inline int32_t destWidth() const { return abs(dstWidth); };
	inline int32_t destHeight() const { return abs(dstHeight); };
	inline AVPixelFormat destFormat() const { return dstPixelFormat; };
	inline int flag() const { return flags; };

	FFmpegScaleParams(){}

	void reset(int32_t nsw, int32_t nsh, DWORD nspfmt,
		int32_t ndw, int32_t ndh, DWORD ndpfmt, int32_t flag)
	{
		srcWidth = nsw;
		srcHeight = nsh;
		srcPixelInFormatFourCC = nspfmt;
		srcPixelFormat = GetPixFmtByFourCC(nspfmt);
		dstWidth = ndw;
		dstHeight = ndh;
		dstPixelInFormatFourCC = ndpfmt;
		dstPixelFormat = GetPixFmtByFourCC(ndpfmt);
		flags = flag;
	}

	bool operator == (FFmpegScaleParams& np)
	{
		return (srcWidth == np.srcWidth
			&& srcHeight == np.srcHeight
			&& srcPixelFormat == np.srcPixelFormat
			&& dstWidth == np.dstWidth
			&& dstHeight == np.dstHeight
			&& dstPixelFormat == np.dstPixelFormat);
	}

	bool operator == (const IVPPPARAMETER& np)
	{
		return (srcWidth == np.srcWidth
			&& srcHeight == np.srcHeight
			&& srcPixelInFormatFourCC == np.srcPixelInFormatFourCC
			&& dstWidth == np.dstWidth
			&& dstHeight == np.dstHeight
			&& dstPixelInFormatFourCC == np.dstPixelInFormatFourCC);
	}

	FFmpegScaleParams &operator = (const IVPPPARAMETER &params)
	{
		reset(params.srcWidth, params.srcHeight, params.srcPixelInFormatFourCC,
			params.dstWidth, params.dstHeight, params.dstPixelInFormatFourCC, params.flags);
		return *this;
	}

private:
	AVPixelFormat srcPixelFormat;
	AVPixelFormat dstPixelFormat;
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

