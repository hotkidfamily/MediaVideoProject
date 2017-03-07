#include "stdafx.h"
#include "PixelFormat.h"
#include "SampleBuffer.h"
#include "utils.h"

E_RES GetResByResolution(int32_t width, int32_t height)
{
	E_RES res = RES1080P;
	int32_t resSize = width*height;

	if (resSize <= GetFrameSizePrePlannerByRes(RES720P)){
		res = RES720P;
	} else if (resSize <= GetFrameSizePrePlannerByRes(RES1080P)){
		res = RES1080P;
	} else if (resSize <= GetFrameSizePrePlannerByRes(RES2K)){
		res = RES2K;
	} else if (resSize <= GetFrameSizePrePlannerByRes(RES4K)){
		res = RES4K;
	} else if (resSize <= GetFrameSizePrePlannerByRes(RES8K)){
		res = RES8K;
	} else {
		res = RES_NONE;
	}

	return res;
}

int32_t GetFrameSizePrePlannerByRes(E_RES res)
{
	int32_t sizePrePlanner = 0;
	int32_t width = 0;
	int32_t height = 0;

	switch (res){
	case RES720P:
		width = WIDTHALIGN(1280);
		height = HEIGHTALIGN(720);
		break;
	case RES2K:
		width = WIDTHALIGN(2048);
		height = HEIGHTALIGN(1080);
		break;
	case RES4K:
		width = WIDTHALIGN(3840);
		height = HEIGHTALIGN(2160);
		break;
	case RES8K:
		width = WIDTHALIGN(7680);
		height = HEIGHTALIGN(4320);
		break;
	default:
	case RES1080P:
		width = WIDTHALIGN(1920);
		height = HEIGHTALIGN(1080);
		break;
	}

	sizePrePlanner = width * height;

	return sizePrePlanner;
}

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

	return *buffer != NULL;
}


void DeallocMemory(uint8_t * ptr)
{
#ifdef DEBUG
	_aligned_free_dbg(ptr);
#else
	_aligned_free(ptr);
#endif
}


BOOL DealloVideoSampleBuffer(VideoSampleBuffer *sample)
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

VideoSampleBuffer *AlloVideoSampleBuffer(int32_t width, int32_t height, CPPixelFormat pixelFormat)
{
	VideoSampleBuffer *sample = nullptr;
	BOOL bRet = FALSE;

	const PIXELFORAMTDESC *bpp = GetPxielFormatDescByFourCC(pixelFormat);

	/* calculate buffer size */
	int32_t rWidth = WIDTHALIGN(width);
	int32_t rHeight = WIDTHALIGN(height);
	int32_t capability = rWidth * rHeight * bpp->pixdesc.bpp >> 3;
	uint8_t *bufferPtr = AllocMemory(capability);

	if (!bufferPtr){
		//logger(Error, "Can not alloc sample buffer.\n");
		goto fail;
	}
	sample = new VideoSampleBuffer();
	assert(sample);
	sample->Reset(bufferPtr, capability, width, height, pixelFormat);

	bRet = TRUE;

fail:
	if (!bRet){
		DealloVideoSampleBuffer(sample);
		sample = NULL;
	}
	
	return sample;
}
