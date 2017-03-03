#include "stdafx.h"
#include "PixelFormat.h"

const PIXELFORAMTDESC pixelFormatDescTable[] = {
	/* YUV 420 */
	{
		PIXEL_FORMAT_I420, 16, 16,
		{
			8, 3, 1, 1, 
			{ { 0, 0 }, { 1, 1 }, { 1, 1 }, { 0, 0 } }
		},
		"yuv420p"
	},
	{
		PIXEL_FORMAT_YV12, 8, 8, 
		{
			8, 2, 1, 0,
			{ { 0, 0 }, { 1, 1 }, { 1, 1 }, { 0, 0 } }
		},
		"yv12"
	},
	{
		PIXEL_FORMAT_NV12, 8, 8,
		{
			8, 2, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"nv12"
	},
	{
		PIXEL_FORMAT_IYUV, 8, 8,
		{
			8, 3, 1, 1, // fix it
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"IYUV"
	},

	/* RGB */
	{
		PIXEL_FORMAT_RGB565, 32, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"RGB565"
	},
	{
		PIXEL_FORMAT_RGB555, 32, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"RGB555"
	},
	{
		PIXEL_FORMAT_RGB24, 32, 2,
		{
			24, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"RGB24"
	},
	{
		PIXEL_FORMAT_RGB32, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"RGB32"
	},
	{
		PIXEL_FORMAT_ARGB, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"ARGB"
	},

	/* YUV 4:2:2 */
	{
		PIXEL_FORMAT_YUYV, 16, 2,
		{
			16, 1, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"YUYV"
	},
	{
		PIXEL_FORMAT_Y411, 16, 2,
		{
			16, 3, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"Y411"
	},
	{
		PIXEL_FORMAT_Y41P, 16, 2,
		{
			16, 3, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"Y41P"
	},
	{
		PIXEL_FORMAT_YUY2, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"YUY2"
	},
	{
		PIXEL_FORMAT_YVYU, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"YVYU"
	},
	{
		PIXEL_FORMAT_UYVY, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"UYVY"
	},

	/* YUV 4:4:4 */
	{
		PIXEL_FORMAT_AYUV, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		"AYUV"
	},
};

const PIXELFORAMTDESC* GetPxielFormatDescByFourCC(DWORD pixelFormat)
{
	int32_t bitsPerFixel = 0;
	const PIXELFORAMTDESC *info = nullptr;
	for (int i = 0; i < ARRAYSIZE(pixelFormatDescTable); i++){
		if (pixelFormatDescTable[i].pixelFormatInFourCC == pixelFormat){
			info = &pixelFormatDescTable[i];
			break;
		}
	}

	return info;
}

