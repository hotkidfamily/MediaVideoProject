#pragma once

#include "IVPP.h"
#include "FFmpegWrapper.h"

typedef struct ctxParams{
	int32_t srcWidth;
	int32_t srcHeight;
	AVPixelFormat spFormat;
	int32_t dstWidth;
	int32_t dstHeight;
	AVPixelFormat dpFormat;
	uint32_t flags; /* no scale not care */

	/**/
	inline int32_t sourceWidth() const { return abs(srcWidth); };
	inline int32_t sourceHeight() const { return abs(srcHeight); };
	inline AVPixelFormat sourceFormat() const { return spFormat; };
	inline int32_t destWidth() const { return abs(dstWidth); };
	inline int32_t destHeight() const { return abs(dstHeight); };
	inline AVPixelFormat destFormat() const { return dpFormat; };
	inline int flag() const { return flags; };

	ctxParams(){} /* do nothing */

	ctxParams(int32_t nsw, int32_t nsh, AVPixelFormat nspfmt,
		int32_t ndw, int32_t ndh, AVPixelFormat ndpfmt, int32_t flag)
		:srcWidth(nsw), srcHeight(nsh), spFormat(nspfmt),
		dstWidth(ndw), dstHeight(ndh), dpFormat(ndpfmt), flags(flag){};

	void reset(int32_t nsw, int32_t nsh, AVPixelFormat nspfmt,
		int32_t ndw, int32_t ndh, AVPixelFormat ndpfmt, int32_t flag)
	{
		ZeroMemory(this, sizeof(struct ctxParams));
		srcWidth = nsw;
		srcHeight = nsh;
		spFormat = nspfmt;
		dstWidth = ndw;
		dstHeight = ndh;
		dpFormat = ndpfmt;
		flags = flag;
	}

	bool operator == (struct ctxParams& np){
		return (srcWidth == np.srcWidth
			&& srcHeight == np.srcHeight
			&& spFormat == np.spFormat
			&& dstWidth == np.dstWidth
			&& dstHeight == np.dstHeight
			&& dpFormat == np.dpFormat);
	}
}FFmpegColorConvertParams;

class FFmpegProcess : public IVPP
{
public:
	FFmpegProcess();
	~FFmpegProcess();

	virtual BOOL InitiaContext(FFmpegColorConvertParams params);

	virtual BOOL DeinitContext();

	virtual BOOL ProcessFrame(uint8_t* sBuf[4], int sStride[4], uint8_t* dBuf[4], int dStride[4]);

private:
	SwsContext *mScaleCtx;
	FFmpegColorConvertParams mParams;
};

