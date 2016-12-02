#pragma once

#include <stdint.h>
#include <list>
#include "PixelFormat.h"
#include "samplebuffer.h"
#include "PackageBuffer.h"
#include "PackageManager.h"
#include "utils.h"

#include "IVideoCodec.h"
#include "IVPP.h"

extern "C" {
	#include "x264.h"
}

class CLibx264 : public ICodec
{
public:
	CLibx264();
	~CLibx264();

	/* step 2 */
	bool open();
	/* step 4 */
	void close();

	/* step 1 */
	bool setConfig(const ENCODECCFG &config);
	/* step 3.x */
	bool reset(const ENCODECCFG &config);

	/* step 3.x */
	bool addFrame(const CSampleBuffer &inputFrame);
	/* step 3.x */
	bool getPackage(CPackageBuffer *&outputPackage);

	bool releasePackage(CPackageBuffer *&outputPackage);
	/* step all */
	uint32_t getLastError() { return mLastError; }

	const char* getCodecDescriptor() { return "libx264"; };

	CodecID getCodecId() { return VCODEC_H264; };

protected:
	void setLastError(uint32_t v){ mLastError = v; };
	bool parseConfigString();
	void flushEncodeCache();
	bool assemblePackage(int outputNALsDataSizeInBytes,
						const x264_nal_t *outputNalus,
						int outputNaluCnt,
						const x264_picture_t *outputPic);
	bool encodeFrame(x264_picture_t *inpic);

private:
	x264_param_t mCodecParams;
	x264_t *mCodecHandle;
	uint32_t mLastError;
	x264_picture_t mInPic;

	CPackageBufferManager mPackages;

	ENCODECCFG mWorkConfig;


	BOOL mbNeedVpp;
	IVPPPARAMETER vppParams;
	IVPPFactory *mVppFactory;
	IVPP * mVpp;
};