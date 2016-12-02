#pragma once

#include <dshow.h>
#include <d3d9.h>
extern "C" {
#include "libswscale\swscale.h"
}

#include "mtype.h"

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

typedef struct tagFrameFormatInfo{
	GUID subtype;
	DWORD pixelFormatInFourCC;
	int priority;
	int32_t bytePerPixel;
	int32_t planeCnt;
	uint32_t planeStride[4];
}FRAMEFORAMTINFO;


typedef struct tagFrameBility{
	enum ability{
		SU_RES = 1 << 15,
		SU_FPS = 1 << 14,
		SU_RES_RATIO = 1 << 13,
		SU_FPS_SMALL = 1 << 12,

		SU_RES_LARGE = 1 << 11,
		SU_FPS_LARGE = 1 << 10,

		SU_RES_LARGE_INAREA = 1 << 9,
		SU_RES_SMALL_INAREA = 1 << 8,
	};
	int32_t Priority;
	int32_t Ability;
	SIZE ImageSize;
	LONGLONG MinFrameInterval;
	LONGLONG MaxFrameInterval;
	CMediaType MediaType;
	tagFrameBility(){
		ZeroMemory(this, sizeof(tagFrameBility));
	}
}FRAMEABILITY, *PFRAMEABILITY;

extern "C" {
	const FRAMEFORAMTINFO* GetFrameInfoByFourCC(DWORD fourcc);
	BOOL IsFormatSupport(CMediaType &mediaType, FRAMEABILITY & bility);
	D3DFORMAT GetD3D9FormatByFourCC(DWORD);
}