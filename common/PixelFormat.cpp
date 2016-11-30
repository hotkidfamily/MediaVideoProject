#include "stdafx.h"
#include <stdint.h>
#include <dshow.h>
#include "PixelFormat.h"

/*
Reference :https://msdn.microsoft.com/zh-cn/library/windows/desktop/dd391027(v=vs.85).aspx
MEDIASUBTYPE_YV12 YV12 4:2:0 Planar 8
MEDIASUBTYPE_I420 I420 4:2:0 Planar 8
MEDIASUBTYPE_NV12 NV12 4:2:0 Planar 8
MEDIASUBTYPE_IYUV IYUV 4:2:0 Planar 8

MEDIASUBTYPE_IMC1 IMC1 4:2:0 Planar 8
MEDIASUBTYPE_IMC3 IMC2 4:2:0 Planar 8
MEDIASUBTYPE_IMC2 IMC3 4:2:0 Planar 8
MEDIASUBTYPE_IMC4 IMC4 4:2:0 Planar 8

MEDIASUBTYPE_YUY2 YUY2 4:2:2 Packed 8
MEDIASUBTYPE_UYVY UYVY 4:2:2 Packed 8
MEDIASUBTYPE_YVYU YVYU 4:2:2 Packed 8

MEDIASUBTYPE_AYUV AYUV 4:4:4 Packed 8
*/
#ifndef MEDIASUBTYPE_I420
const GUID MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
#endif

const FRAMEFORAMTINFO videoFormatTable[] = {
	/* YUV 420 */
	{ MEDIASUBTYPE_I420, PIXEL_FORMAT_I420, 1 << 15, 8, 3, { 0, 2, 2, 0 } },
	{ MEDIASUBTYPE_YV12, PIXEL_FORMAT_YV12, 1 << 15, 8, 2, { 0, 1, 0, 0 } },
	{ MEDIASUBTYPE_NV12, PIXEL_FORMAT_NV12, 1 << 15, 8, 2, { 0, 1, 0, 0 } },
	{ MEDIASUBTYPE_IYUV, PIXEL_FORMAT_IYUV, 1 << 14, 8, 2, { 0, 1, 0, 0 } },

	/* MJPEG*/
	{ MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG, 1 << 14, 1, 1 },

	/* RGB */
	{ MEDIASUBTYPE_RGB565, PIXEL_FORMAT_RGB565, 1 << 13, 16, 1 },
	{ MEDIASUBTYPE_RGB555, PIXEL_FORMAT_RGB555, 1 << 13, 16, 1 },
	{ MEDIASUBTYPE_RGB24, PIXEL_FORMAT_RGB24, 1 << 12, 24, 1 },
	{ MEDIASUBTYPE_RGB32, PIXEL_FORMAT_RGB32, 1 << 14, 32, 1 },
	{ MEDIASUBTYPE_ARGB32, PIXEL_FORMAT_ARGB, 1 << 14, 32, 1 },

	/* YUV 4:2:2 */
	{ MEDIASUBTYPE_YUYV, PIXEL_FORMAT_YUYV, 1 << 13, 16, 1, { 0, 0, 0, 0 } }, /*422 16bits per pixel top-down image, YUYV packet */
	{ MEDIASUBTYPE_YVU9, PIXEL_FORMAT_YVU9, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y411, PIXEL_FORMAT_Y411, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y41P, PIXEL_FORMAT_Y41P, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_YVYU, PIXEL_FORMAT_YVYU, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y211, PIXEL_FORMAT_Y211, 1 << 13, 16, 2 },
	{ MEDIASUBTYPE_NV11, PIXEL_FORMAT_NV11, 1 << 13, 16, 2 },
	{ MEDIASUBTYPE_420O, PIXEL_FORMAT_420O, 1 << 13, 16, 3 },

	/* YUV 4:4:4 */
	{ MEDIASUBTYPE_AYUV, PIXEL_FORMAT_AYUV, 1 << 10, 32, 1 }, /*AYUV4:4:4 Packed 8*/
};


const FRAMEFORAMTINFO* GetFrameInfoByFourCC(DWORD pixelFormat)
{
	int32_t bitsPerFixel = 0;
	const FRAMEFORAMTINFO *info = NULL;
	for (int i = 0; i < ARRAYSIZE(videoFormatTable); i++){
		if (videoFormatTable[i].pixelFormatInFourCC == pixelFormat){
			info = &videoFormatTable[i];
			break;
		}
	}

	return info;
}

BOOL IsFormatSupport(CMediaType &mediaType, FRAMEABILITY & bility)
{
	BOOL bRet = FALSE;
	for (int i = 0; i < ARRAYSIZE(videoFormatTable); i++){
		if (mediaType.subTypeEqual(videoFormatTable[i].subtype)){
			bility.Priority = videoFormatTable[i].priority;
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

