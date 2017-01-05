#pragma once

#include <stdint.h>

// capture output format
#undef FMT
#define FMT(X,Y) PIXEL_FORMAT_##X=Y
enum CPPixelFormat{
	FMT(I420, '024I'), 
	FMT(RGB565, 0xe436eb7c),
	FMT(RGB555, 0xe436eb7b),
	FMT(RGB24, 0xe436eb7d),
	FMT(RGB32, 0xe436eb7e),
	FMT(ARGB, 0x773c9ac0),
	FMT(YV12, '21VY'), //YVU 4:2:0 planar == YUV420p
	FMT(NV12, '21VN'), //YUV 4:2:0, with one y plane and one packed u+v
	FMT(IYUV, 'VYUI'),
	FMT(MJPEG, 'GPJM'),
	FMT(YUYV, 'VYUY'),
	FMT(YVU9, '9UVY'),
	FMT(Y411, '114Y'),
	FMT(Y41P, 'P14Y'),
	FMT(YUY2, '2YUY'), //YUV 4:2:2, with y+u+y+v packed
	FMT(YVYU, 'UYVY'), //YUV 4:2:2, with Y+V+Y+U packed top bottom
	FMT(UYVY, 'YVYU'),
	FMT(Y211, '112Y'),
	FMT(NV11, '11VN'),
	FMT(420O, 'O024'),
	FMT(AYUV, 0x56555941)
};
#undef FMT

typedef struct tagPixelFmtInfo{
	int32_t bpp; // bits per pixel
	int32_t planarCnt; // 1 ~ 4 LIKE ARGB XRGB AYUV YUV
	int32_t chromaWShift;
	int32_t chromaHShift;
	struct {
		int32_t widthShift;
		int32_t heightShift;
	} resShift[4];
}PIXFMTINFO;

//{ 16, 3, { {},{},{},{} }}
typedef struct tagFrameFormatInfo{
	DWORD pixelFormatInFourCC;
	int32_t strideW;
	int32_t strideH;
	PIXFMTINFO pixdesc;
}PIXELFORAMTDESC;

extern "C" {
	const PIXELFORAMTDESC* GetPxielFormatDescByFourCC(DWORD fourcc);
}