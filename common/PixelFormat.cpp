#include "stdafx.h"
#include <stdint.h>
#include <dshow.h>
#include "PixelFormat.h"

const FRAMEFORAMTINFO videoFormatTable[] = {
	/* YUV 420 */
	{ PIXEL_FORMAT_I420, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_YV12, { 8, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_NV12, { 8, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_IYUV, { 8, 3, { {}, {}, {}, {} } } },

	/* RGB */
	{ PIXEL_FORMAT_RGB565, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_RGB555, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_RGB24, { 24, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_RGB32, { 32, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_ARGB, { 32, 3, { {}, {}, {}, {} } } },

	/* YUV 4:2:2 */
	{ PIXEL_FORMAT_YUYV, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_Y411, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_Y41P, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_YUY2, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_YVYU, { 16, 3, { {}, {}, {}, {} } } },
	{ PIXEL_FORMAT_UYVY, { 16, 3, { {}, {}, {}, {} } } },

	/* YUV 4:4:4 */
	{ PIXEL_FORMAT_AYUV, { 32, 3, { {}, {}, {}, {} } } },
};

const FRAMEFORAMTINFO* GetFrameInfoByFourCC(DWORD pixelFormat)
{
	int32_t bitsPerFixel = 0;
	const FRAMEFORAMTINFO *info = nullptr;
	for (int i = 0; i < ARRAYSIZE(videoFormatTable); i++){
		if (videoFormatTable[i].pixelFormatInFourCC == pixelFormat){
			info = &videoFormatTable[i];
			break;
		}
	}

	return info;
}

