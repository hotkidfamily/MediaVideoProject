#include "stdafx.h"
#include "IVPP.h"
#include "VppUtils.h"
#include "PixelFormat.h"
#include <ddraw.h>

typedef struct tagPixelFormatFFmpegFourCC{
	DWORD pfFourCC;
	AVPixelFormat fpFFmpeg;
}PFFF;

PFFF PFTable[] = {
	{ PIXEL_FORMAT_I420, AV_PIX_FMT_YUV420P },
	{ PIXEL_FORMAT_YV12, AV_PIX_FMT_YUV420P },
	{ PIXEL_FORMAT_NV12, AV_PIX_FMT_NV12 },
	{ PIXEL_FORMAT_IYUV, AV_PIX_FMT_YUV420P },
	{ PIXEL_FORMAT_MJPEG, AV_PIX_FMT_NONE },
	{ PIXEL_FORMAT_RGB565, AV_PIX_FMT_RGB565BE },
	{ PIXEL_FORMAT_RGB555, AV_PIX_FMT_RGB555BE },
	{ PIXEL_FORMAT_RGB24, AV_PIX_FMT_RGB24 },
	{ PIXEL_FORMAT_RGB32, AV_PIX_FMT_RGB32 },
	{ PIXEL_FORMAT_ARGB, AV_PIX_FMT_ARGB },
	{ PIXEL_FORMAT_YUYV, AV_PIX_FMT_YUYV422 },
	{ PIXEL_FORMAT_Y411, AV_PIX_FMT_YUV411P },
	{ PIXEL_FORMAT_Y41P, AV_PIX_FMT_YUV410P },
	{ PIXEL_FORMAT_YUY2, AV_PIX_FMT_YUYV422 },
	{ PIXEL_FORMAT_YVYU, AV_PIX_FMT_YVYU422 },
	{ PIXEL_FORMAT_UYVY, AV_PIX_FMT_UYVY422 },
	{ PIXEL_FORMAT_AYUV, AV_PIX_FMT_YUVA444P },
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