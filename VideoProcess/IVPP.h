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
protected:
	~IVPP(){};

public:
	virtual BOOL InitContext(IVPPPARAMETER) = 0;
	virtual BOOL DeinitContext() = 0;

	virtual BOOL ProcessFrame(const CSampleBuffer *, CSampleBuffer *) = 0;
};

class IVPPFactory
{
protected:
	~IVPPFactory(){};

public:
	virtual BOOL CreateVPPObj(IVPP*&) = 0;
	virtual void DestoryVPPObj(IVPP *) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif

	VIDEOPROCESS_API BOOL GetVPPFactoryObj(IVPPFactory * &);
	VIDEOPROCESS_API BOOL ReleaseVPPFctoryObj(IVPPFactory *);

#ifdef __cplusplus
}
#endif
