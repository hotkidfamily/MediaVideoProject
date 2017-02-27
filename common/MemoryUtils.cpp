#include "stdafx.h"
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

bool ReallocMemory(uint8_t** buffer, uint32_t sizeInByte)
{
	uint8_t *temp = NULL;
#ifdef DEBUG
	temp = (uint8_t*)_aligned_malloc_dbg(sizeInByte, 32);
#else
	temp = (uint8_t*)_aligned_malloc(sizeInByte, 32);
#endif

	DeallocMemory(*buffer);
	*buffer = NULL;

	*buffer = temp;

	return *buffer == NULL;
}


void DeallocMemory(uint8_t * ptr)
{
#ifdef DEBUG
	_aligned_free_dbg(ptr);
#else
	_aligned_free(ptr);
#endif
}


BOOL DeallocSampleBuffer(CSampleBuffer *sample)
{
	if (!sample){
		goto done;
	}

	uint8_t* ptr = sample->GetDataPtr();

	DeallocMemory(ptr);

	delete sample;

done:
	return TRUE;
}

CSampleBuffer *AllocSampleBuffer(int32_t width, int32_t height, CPPixelFormat pixelFormat)
{
	CSampleBuffer *sample = new CSampleBuffer;
	BOOL bRet = FALSE;

	const PIXELFORAMTDESC *bpp = GetPxielFormatDescByFourCC(pixelFormat);

	/* calc buffer size */
	int32_t rWidth = WIDTHALIGN(width);
	int32_t rHeight = WIDTHALIGN(height);
	int32_t capability = rWidth * rHeight * bpp->pixdesc.bpp >> 3;
	uint8_t *bufferPtr = AllocMemory(capability);

	if (!sample || !bufferPtr){
		goto fail;
	}

	sample->Reset(bufferPtr, capability, width, height, pixelFormat);

	bRet = TRUE;

fail:
	if (!bRet){
		DeallocSampleBuffer(sample);
		sample = NULL;
	}
	
	return sample;
}
