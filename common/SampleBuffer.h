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

enum ColorRange
{
	ColorRange_Full = 0, // 0~255;0~1023,10bit;also define in PC
	ColorRange_Video = 1, // 16~235；64~940；also mean TV
	/*ColorPrimaries_smtp, */// 48~208, 192~832
};

enum ColorPrimaries
{
	ColorPrimaries_BT709 = 1,
	ColorPrimaries_Unspecified,
	ColorPrimaries_BT470M = 4,
	ColorPrimaries_BT470BG,
	ColorPrimaries_SMPTE170M,
	ColorPrimaries_SMPTE240M,
	ColorPrimaries_Film,
	ColorPrimaries_BT2020
};

enum ColorTransfer
{
	ColorTransfer_BT709 = 1,
	ColorTransfer_Unspecified,
	ColorTransfer_BT470M = 4,
	ColorTransfer_BT470BG,
	ColorTransfer_SMPTE170M,
	ColorTransfer_SMPTE240M,
	ColorTransfer_Linear,
	ColorTransfer_Log100,
	ColorTransfer_Log316,
	ColorTransfer_IEC6196624,
	ColorTransfer_BT1361,
	ColorTransfer_IEC6196621,
	ColorTransfer_BT202010,
	ColorTransfer_BT202012
};

enum ColorMatrix
{
	ColorMatrix_GBR = 0,
	ColorMatrix_BT709,
	ColorMatrix_Unspecified,
	ColorMatrix_BT470M = 4,
	ColorMatrix_BT470BG,
	ColorMatrix_SMPTE170M,
	ColorMatrix_SMPTE240M,
	ColorMatrix_YCgCo,
	ColorMatrix_BT2020NCL,
	ColorMatrix_BT2020CL
};


typedef struct tagFrameDesc{
	int32_t cbSize;
	int32_t width;
	int32_t height;
	int32_t dataSize;
	uint8_t *dataPtr;
	DWORD pixelFormatInFourCC;
	int32_t lineSize;
	int64_t ptsStart;
	int64_t ptsEnd;
	int64_t frameStartIdx;
	int64_t frameEndIdx;
	tagFrameDesc(){
		ZeroMemory(this, sizeof(struct tagFrameDesc));
		cbSize = sizeof(struct tagFrameDesc);
	}
}FRAME_DESC;

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
	int32_t colorRange;
	int32_t transferMatrix;
	int32_t primaries;
};

