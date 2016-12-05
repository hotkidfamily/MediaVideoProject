#include "stdafx.h"
#include <windows.h>
#include "PixelFormat.h"
#include "SampleBuffer.h"

#define ALIGN32(x) ((x + 0x1F) & (~0x1F))
#define ALIGN2(x) ((x + 0x1) & (~0x1))

CSampleBuffer* AllocSampleBuffer(int width, int height, DWORD pixelFormat)
{
	CSampleBuffer* sample = NULL;
	const FRAMEFORAMTINFO *bpp = GetFrameInfoByFourCC(pixelFormat);
	int32_t realWidth = ALIGN32(width);
	int32_t realHeight = ALIGN2(height);
	int32_t sizeInBytes = realWidth*realHeight*	bpp->bytePerPixel;

	uint8_t *bufferptr = (uint8_t*)_aligned_malloc(sizeInBytes, 32);

	return sample;
}