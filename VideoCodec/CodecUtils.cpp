#include "stdafx.h"
#include <ddraw.h>
extern "C" {
#include "x264.h"
}
#include "CodecUtils.h"

typedef struct tagPixelFormatX264AndFourCC
{
	int32_t pfX264;
	DWORD pfFourCC;
}PFAVCFCC;

PFAVCFCC PFTable[] = {
		{ X264_CSP_I420, MAKEFOURCC('I', '4', '2', '0') },
		{ X264_CSP_YV12, MAKEFOURCC('Y', 'V', '1', '2') },
		{ X264_CSP_NV12, MAKEFOURCC('N', 'V', '1', '2') },
		{ X264_CSP_I422, MAKEFOURCC('I', '4', '2', '2') },
		{ X264_CSP_YV16, MAKEFOURCC('Y', 'V', '1', '6') },
		{ X264_CSP_NV16, MAKEFOURCC('N', 'V', '1', '6') },
		{ X264_CSP_V210, MAKEFOURCC('V', '2', '1', '0') },
		{ X264_CSP_I444, MAKEFOURCC('I', '4', '4', '4') },
		{ X264_CSP_YV24, MAKEFOURCC('Y', 'V', '2', '4') },
		{ X264_CSP_BGR, 0xe436eb7e }, // not support
		{ X264_CSP_BGRA, 0x773c9ac0 },
		{ X264_CSP_RGB, 0xe436eb7d },
};


int32_t TranslateFourCCToLibX264Format(DWORD pfFourCC)
{
	int32_t fp = X264_CSP_NONE;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].pfFourCC == pfFourCC){
			fp = PFTable[i].pfX264;
		}
	}
	return fp;
}

DWORD TranslateLibX264FormatToFourCC(int32_t format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].pfX264 == format){
			fourCC = PFTable[i].pfFourCC;
		}
	}
	return fourCC;
}