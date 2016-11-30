#pragma once

#ifdef VIDEOPROCESS_EXPORTS
#define VIDEOPROCESS_API __declspec(dllexport)
#else
#define VIDEOPROCESS_API __declspec(dllimport)
#endif

#include <stdint.h>
#include "SampleBuffer.h"

typedef struct IVPPContext{
	int32_t srcWidth;
	int32_t srcHeight;
	DWORD srcPixelInFormatFourCC;
	int32_t dstWidth;
	int32_t dstHeight;
	DWORD dstPixelInFormatFourCC;
	uint32_t flags;
}IVPPPARAMETER, *PIVPPPARAMETER;

class IVPP {
public:
	virtual BOOL InitContext(IVPPPARAMETER) = 0;
	virtual BOOL DeinitContext() = 0;

	virtual BOOL ProcessFrame(const CSampleBuffer *, CSampleBuffer *) = 0;

protected:
	~IVPP(){};
};

class VPPFactory
{
public:
	VPPFactory();
	~VPPFactory();

	IVPP *CreateVPP();
	void DestoryVPP(IVPP *);
};

extern "C" {

	VIDEOPROCESS_API  VPPFactory *GetVPPFactoryObj();
	VIDEOPROCESS_API void ReleaseVPPFctoryObj();

}
