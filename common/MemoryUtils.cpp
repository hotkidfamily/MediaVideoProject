#include "stdafx.h"
#include <windows.h>
#include "PixelFormat.h"
#include "SampleBuffer.h"
#include "utils.h"

uint8_t* AllocMemory(uint32_t sizeInByte)
{
#ifdef DEBUG
	return (uint8_t*)_aligned_malloc_dbg(sizeInByte, 32);
#else
	return (uint8_t*)_aligned_malloc(sizeInByte, 32);
#endif
}

void DeallocMemory(uint8_t * ptr)
{
#ifdef DEBUG
	_aligned_free_dbg(ptr);
#else
	_aligned_free(ptr);
#endif
}

BOOL AllocSampleBuffer(int32_t width, int32_t height, DWORD pixelFormat)
{
	BOOL bRet = FALSE;
	uint32_t planarSize[4] = { 0 };
	uint32_t planarStride[4] = { 0 };
	uint8_t *planarPtr[4] = { NULL };

	const FRAMEFORAMTINFO *bpp = GetFrameInfoByFourCC(pixelFormat);
	int32_t realWidth = WIDTHALIGN(width);
	int32_t realHeight = HEIGHTALIGN(height);
	int32_t sizeInBytes = realWidth * realHeight * bpp->pixdesc.bpp;

	for (int32_t i = 0; i < bpp->pixdesc.planarCnt; i++){
		planarSize[i] = realWidth * realHeight * bpp->pixdesc.resShift[i].wOffset;
		planarPtr[i] = AllocMemory(planarSize[i]);
		if (!planarPtr[i]){
			goto fail;
		}
		planarStride[i] = realWidth * realHeight * bpp->pixdesc.resShift[i].wOffset;
	}

	bRet = TRUE;

fail:
	if (!bRet){
		for (int32_t i = 0; i < bpp->pixdesc.planarCnt; i++){
			if (planarPtr[i] != NULL){
				DeallocMemory(planarPtr[i]);
			}
		}
	}
	
	return FALSE;
}