#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <SampleBuffer.h>
#include <PackageBuffer.h>
#include "utils.h"

#ifdef VIDEOCODEC_EXPORTS
#define VIDEOCODEC_API __declspec(dllexport)
#else
#define VIDEOCODEC_API __declspec(dllimport)
#endif

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
}ENCODECCFG, *PENCODECCFG;

class ICodec{
protected:
	~ICodec(){};

public:
	/* step 2 */
	virtual bool open() = 0;

	/* step 4 */
	virtual void close() = 0;

	/* step 1 */
	virtual bool setConfig(const ENCODECCFG &config) = 0;
	/* step 3.x */
	virtual bool reset(const ENCODECCFG &config) = 0;

	/* step 3.x */
	virtual bool addFrame(const CSampleBuffer &inputFrame) = 0;
	/* step 3.x */
	virtual bool getPackage(CPackageBuffer *&outputPackage) = 0;

	/* step 3.(x+1) */
	virtual bool releasePackage(CPackageBuffer *&outputPackage) = 0;

	/* step all */
	virtual uint32_t getLastError() = 0;
};

class ICodecFactory
{
protected:
	~ICodecFactory(){};

public:
	virtual BOOL CreateCodec(ICodec* &codec) = 0;
	virtual BOOL DestoryCodec(ICodec *&codec) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif

	VIDEOCODEC_API BOOL GetCodecFactoryOBj(ICodecFactory* &);
	VIDEOCODEC_API BOOL ReleaseCodecFactoryOBj(ICodecFactory *);

#ifdef __cplusplus
}
#endif