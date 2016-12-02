#include "stdafx.h"
#include "IVPP.h"
#include "VppUtils.h"
#include "PixelFormat.h"
#include <ddraw.h>

typedef struct tagPixelFormatFFmpegFourCC
{
	DWORD pfFourCC;
	AVPixelFormat fpFFmpeg;
}PFFF;

PFFF PFTable[] = {
	{ PIXEL_FORMAT_I420, AV_PIX_FMT_YUV420P },
	{ PIXEL_FORMAT_YUYV, AV_PIX_FMT_YUV422P },
	{ PIXEL_FORMAT_RGB24, AV_PIX_FMT_RGB24 },
	{ PIXEL_FORMAT_RGB32, AV_PIX_FMT_RGB32 },
	{ PIXEL_FORMAT_NV12, AV_PIX_FMT_NV12 },
};

AVPixelFormat GetPixFmtByFourCC(DWORD pfFourCC)
{
	AVPixelFormat fp = AV_PIX_FMT_NONE;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].pfFourCC == pfFourCC){
			fp = PFTable[i].fpFFmpeg;
		}
	}
	return fp;
}

DWORD GetFourCCByPixFmt(AVPixelFormat format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PFTable); i++){
		if (PFTable[i].fpFFmpeg == format){
			fourCC = PFTable[i].pfFourCC;
		}
	}
	return fourCC;
}