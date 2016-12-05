#include "stdafx.h"
#include <stdint.h>
#include <dshow.h>
#include "PixelFormat.h"

const PIXELFORAMTDESC pixelFormatDescTable[] = {
	/* YUV 420 */
	{
		PIXEL_FORMAT_I420, 16, 16,
		{
			8, 3, 1, 1, 
			{ { 0, 0 }, { 1, 1 }, { 1, 1 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_YV12, 8, 8, 
		{
			8, 2, 1, 0,
			{ { 0, 0 }, { 1, 1 }, { 1, 1 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_NV12, 8, 8,
		{
			8, 2, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_IYUV, 8, 8,
		{
			8, 3, 1, 1, // fix it
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},

	/* RGB */
	{
		PIXEL_FORMAT_RGB565, 32, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_RGB555, 32, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_RGB24, 32, 2,
		{
			24, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_RGB32, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_ARGB, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},

	/* YUV 4:2:2 */
	{
		PIXEL_FORMAT_YUYV, 16, 2,
		{
			16, 1, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_Y411, 16, 2,
		{
			16, 3, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_Y41P, 16, 2,
		{
			16, 3, 1, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_YUY2, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_YVYU, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},
	{
		PIXEL_FORMAT_UYVY, 16, 2,
		{
			16, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
	},

	/* YUV 4:4:4 */
	{
		PIXEL_FORMAT_AYUV, 32, 2,
		{
			32, 1, 0, 0,
			{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
		}
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

