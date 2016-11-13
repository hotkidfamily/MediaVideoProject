#pragma once

#include "AutoLock.h"

/*
step 1: lock
step 2: reset 
step 3: fill data
step 4.x get buffer
step 4.x get pts
step 5: unlock
*/
class CSampleBuffer
{
public:
	CSampleBuffer()
		: occupy(FALSE)
		, pts(0)
		, pixelFormat(0)
		, capacity(0)
		, sizeInUse(0)
		, dataPtr(NULL)
	{

	};

	~CSampleBuffer(){};

	/* for constructor, should first call */
	BOOL Reset(uint8_t *bufferPtr, int32_t capacityInBytes){
		if (occupy)
			return FALSE;

		this->pts = 0;
		this->sizeInUse = 0;
		this->capacity = capacityInBytes;
		this->pixelFormat = 0;
		this->dataPtr = bufferPtr;
		this->occupy = FALSE;

		return TRUE;
	}

	BOOL FillData(FRAME_DESC desc){
		CAutoLock lock;
		if (this->occupy || desc.dataSize > this->capacity || !desc.dataPtr){
			return FALSE;
		}

		this->sizeInUse = desc.dataSize;
		memcpy_s(this->dataPtr, this->capacity, desc.dataPtr, desc.dataSize);
		this->pixelFormat = pixelFormat;
		this->pts = pts;
		this->width = desc.width;
		this->height = desc.height;
		this->occupy = TRUE;

		return TRUE;
	}

	uint32_t GetDataSize() const { return sizeInUse; };
	uint8_t *GetDataPtr() const { return dataPtr; };
	int64_t  GetPts() const { return pts; };
	int32_t  GetPixelFormat() const{ return pixelFormat; };

	BOOL LockBuffer() { return mMainLock.Lock(); };
	VOID UnlockBuffer() { mMainLock.Unlock(); };

private:
	BOOL occupy;
	int32_t width;
	int32_t height;
	int64_t pts;
	int32_t pixelFormat;
	int32_t capacity; // size of buffer 
	int32_t sizeInUse; // size of data
	uint8_t *dataPtr;
	CLock mMainLock;
};

