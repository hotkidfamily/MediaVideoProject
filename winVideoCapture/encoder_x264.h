
#include <list>
#include "samplebuffer.h"

extern "C" {
#include <stdint.h>
#include "x264.h"
}

typedef struct tagEncodecConfig
{
	tagEncodecConfig(){
		memset(this, 0, sizeof(struct tagEncodecConfig));
	}

	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t avgBitrateInKb;
	uint32_t maxBitrateInKb;
	uint32_t minBitrateInKb;
	uint32_t pixelFormatInFourCC;
	// pps sps vps etc.
	uint8_t *extraData;
	uint32_t extraDataSize;
	// config string etc.
	STRING cfgStr;
	// reserved for future
	void *extraParams;
}ENCODEC_CFG, *PENCODEC_CFG;

struct DwVideoPackage
{
	DwVideoPackage() {
		memset(this, 0, sizeof(struct DwVideoPackage));
	}

	void Release() {
		if (packageData) {
			delete[] packageData;
			packageData = NULL;
		}
		packageDataSize = 0;
		if (extraData){
			delete[] extraData;
			extraData = NULL;
		}
		extraDataSize = 0;
	}

	enum FRAME_TYPE{
		ERR_FRAME = 0,
		P_FRAME = 1,
		I_FRAME = 2,
		IDR_FRAME = 3,
		B_FRAME = 4
	};

	bool isIDRFrame() const { return frameType == IDR_FRAME; }
	bool isIFrame() const{ return frameType == I_FRAME; }
	bool isBFrame() const { return frameType == B_FRAME; }
	bool isPFrame() const { return frameType == P_FRAME; }
	bool isErrFrame() const { return frameType == ERR_FRAME; }

	uint8_t *packageData;
	uint32_t packageDataSize;
	uint8_t *extraData;
	uint32_t extraDataSize;
	FRAME_TYPE frameType;
	int64_t pts;
	int64_t dts; /* sometimes dts is negative value */

	// reserved for future
	void *extraParam;
};

class CLibx264{
public:
	CLibx264();
	~CLibx264();

	/* step 2 */
	bool open();
	/* step 4 */
	void close();

	/* step 1 */
	bool setConfig(const ENCODEC_CFG &config);
	/* step 3.x */
	bool reset(const ENCODEC_CFG &config);

	/* step 3.x */
	bool addFrame(const CSampleBuffer &inputFrame);
	/* step 3.x */
	bool getPackage(DwVideoPackage &outputPackage);
	/* step all */
	uint32_t getLastError() { return mLastError; }

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

	std::list<DwVideoPackage> mPackages;

	ENCODEC_CFG mWorkConfig;
	CLock lock;
};