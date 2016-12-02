#include "stdafx.h"
#include "IVPP.h"
#include "VppUtils.h"
#include <ddraw.h>


typedef struct tagPixelFormatFFmpegFourCC
{
	AVPixelFormat fpFFmpeg;
	DWORD pfFourCC;
}PFFF;

PFFF PFTable[] = {
		{ AV_PIX_FMT_YUV420P, MAKEFOURCC('Y', 'U', 'V', '0') },
};

AVPixelFormat TranslateFourCCToFFmpegPixelFormat(DWORD pfFourCC)
{
	AVPixelFormat fp = AV_PIX_FMT_NONE;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].pfFourCC == pfFourCC){
			fp = PFTable[i].fpFFmpeg;
		}
	}
	return fp;
}

DWORD TranslateFFmpegPixelFormatToFourCC(AVPixelFormat format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].fpFFmpeg == format){
			fourCC = PFTable[i].pfFourCC;
		}
	}
	return fourCC;
}