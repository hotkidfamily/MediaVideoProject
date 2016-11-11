#pragma once

#include "AutoLock.h"

/*
step 1: lock
step 2: reset 
step 3: filldata
step 4.x getbuffer
step 4.x getpts
step 5: unlock
*/
class CSampleBuffer
{
public:
	CSampleBuffer(){};
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

	BOOL FillData(uint8_t *dataBuf, int32_t dataSize, int64_t pts, int32_t pixelFormat){
		CAutoLock lock;
		if (occupy || dataSize > capacity || !dataBuf){
			return FALSE;
		}

		this->sizeInUse = dataSize;
		memcpy_s(dataPtr, capacity, dataBuf, sizeInUse);
		this->pixelFormat = pixelFormat;
		this->pts = pts;
		occupy = TRUE;

		return TRUE;
	}

	uint32_t GetDataSize() const { return sizeInUse; };
	uint8_t *GetDataPtr() const { return dataPtr; };
	int64_t  GetPts() const { return pts; };
	int32_t  GetPixelFormat() const{ return pixelFormat; };

	BOOL LockBuffer() { mMainLock.Lock(); }
	BOOL UnlockBuffer() { mMainLock.Unlock(); }

private:
	BOOL occupy;
	int64_t pts;
	int32_t pixelFormat;
	int32_t capacity; // size of buffer 
	int32_t sizeInUse; // size of data
	uint8_t *dataPtr;
	CLock mMainLock;
};

