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

typedef enum tagPixelFormat{
	PIXEL_FORMAT_I420 = 'I420', //yuv 4:2:0 planar
	PIXEL_FORMAT_RGB24 = 0xe436eb7d,
	PIXEL_FORMAT_RGB32 = 0xe436eb7e,
	PIXEL_FORMAT_YV12 = 'YV12', //yvu 4:2:0 planar
	PIXEL_FORMAT_NV12 = 'NV12', //yuv 4:2:0, with one y plane and one packed u+v
};


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

		return TRUE;
	}

	uint32_t GetDataSize() const { return sizeInUse; };
	uint8_t *GetDataPtr() const { return dataPtr; };
	int64_t  GetPts() const { return pts; };
	int32_t GetWidth() const { return width; };
	int32_t GetHeight() const { return height; };
	int32_t  GetPixelFormat() const{ return pixelFormat; };

private:
	int32_t width;
	int32_t height;
	int64_t pts;
	int32_t pixelFormat;
	int32_t capacity; // size of buffer 
	int32_t sizeInUse; // size of data
	uint8_t *dataPtr;
};

