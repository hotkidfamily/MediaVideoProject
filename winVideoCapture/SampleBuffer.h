#pragma once

#include "AutoLock.h"

/*
step 1: lock
step 2: reset 
step 3: fill data
step 4.x get buffer
step 4.x get pts
step 5: unlock

attention:
	pixelFormat is pixel format description in FOURCC

*/

class CSampleBuffer
{
public:
	CSampleBuffer()
		: pts(0)
		, pixelFormat(0)
		, capacity(0)
		, sizeInUse(0)
		, dataPtr(NULL){};

	CSampleBuffer(uint8_t *bufferPtr, int32_t capacityInBytes)
		: pts(0)
		, pixelFormat(0)
		, capacity(capacityInBytes)
		, sizeInUse(0)
		, dataPtr(bufferPtr)
	{};

	~CSampleBuffer(){};

	/* for constructor, should first call */
	BOOL Reset(uint8_t *bufferPtr, int32_t capacityInBytes){
		this->pts = 0;
		this->sizeInUse = 0;
		this->capacity = capacityInBytes;
		this->pixelFormat = 0;
		this->dataPtr = bufferPtr;

		return TRUE;
	}

	BOOL FillData(FRAME_DESC desc){
		if (desc.dataSize > this->capacity || !desc.dataPtr){
			return FALSE;
		}

		this->sizeInUse = desc.dataSize;
		memcpy_s(this->dataPtr, this->capacity, desc.dataPtr, desc.dataSize);
		this->pixelFormat = desc.pixelFormatInFourCC;
		this->pts = desc.ptsStart;
		this->width = desc.width;
		this->height = desc.height;
		this->lineSize = desc.lineSize;

		return TRUE;
	}

	inline uint32_t GetDataSize() const { return sizeInUse; };
	inline uint8_t *GetDataPtr() const { return dataPtr; };
	inline int32_t GetLineSize() const { return lineSize; };
	inline int64_t  GetPts() const { return pts; };
	inline int32_t GetWidth() const { return width; };
	inline int32_t GetHeight() const { return height; };
	inline int32_t  GetPixelFormat() const{ return pixelFormat; };

private:
	int32_t width;
	int32_t lineSize;
	int32_t height;
	int64_t pts;
	int32_t pixelFormat;
	int32_t capacity; // size of buffer 
	int32_t sizeInUse; // size of data
	uint8_t *dataPtr;
};

