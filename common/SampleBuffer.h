#pragma once

#include <stdint.h>
#include "PixelFormat.h"
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
	// common
	int32_t cbSize;

	int32_t width;
	int32_t height;

	int64_t ptsStart;
	int64_t ptsEnd;
	int64_t frameStartIdx;
	int64_t frameEndIdx;

	DWORD pixelFormatInFourCC;
	ColorRange colorRange;
	ColorTransfer colorTransfer;
	ColorPrimaries colorPrimaries;
	ColorMatrix colorMatrix;

	uint8_t* dataPtr;
	uint32_t validDataSize;

	tagFrameDesc(){
		reset();
	}

	void reset(){
		ZeroMemory(this, sizeof(struct tagFrameDesc));
		cbSize = sizeof(struct tagFrameDesc);
		colorRange = ColorRange_Full;
		colorTransfer = ColorTransfer_Unspecified;
		colorMatrix = ColorMatrix_Unspecified;
		colorPrimaries = ColorPrimaries_Unspecified;
	}
}FRAME_DESC;

class CSampleBuffer
{
public:
	CSampleBuffer()
		: capacity(0)
		, dataPtr(nullptr)
	{};

	CSampleBuffer(uint8_t *bufferPtr, int32_t capacityInBytes)
		: capacity(capacityInBytes)
		, dataPtr(bufferPtr)
	{};

	~CSampleBuffer(){};

	/* for constructor, should first call */
	BOOL Reset(uint8_t *bufferPtr, int32_t capacityInBytes){
		this->capacity = capacityInBytes;
		this->dataPtr = bufferPtr;
		frameDesc.reset();
		return TRUE;
	}

	BOOL Set(const FRAME_DESC &desc)
	{
		memcpy(&frameDesc, &desc, sizeof(desc));
		return TRUE;
	}

	CSampleBuffer & operator= (const FRAME_DESC &desc){
		Set(desc);

		return *this;
	}

	BOOL FillData(FRAME_DESC desc){
		if (desc.validDataSize > this->capacity || !desc.dataPtr){
			return FALSE;
		}

		frameDesc = desc;
		const FRAMEFORAMTINFO * info = GetFrameInfoByFourCC(desc.pixelFormatInFourCC);

		planarPtr[0] = dataPtr;
		planarStride[0] = ((desc.width * info->pixdesc.bpp + 31)&~32) >> 3;
		planarSize[0] = planarStride[0] * desc.height ;

		for (int32_t i = 1; i < info->pixdesc.planarCnt; i++){
			planarStride[i] = planarStride[i-1] >> info->pixdesc.resShift[i].wOffset;
			planarSize[i] = planarStride[i] * desc.height;
			planarPtr[i] = planarPtr[i] + planarSize[i];
		}

		return TRUE;
	}

	inline uint32_t GetDataSize() const { return frameDesc.validDataSize; };
	inline uint8_t *GetDataPtr() const { return dataPtr; };
	inline uint8_t* *GetPlanarPtr() const { return (uint8_t**)planarPtr; };
	inline int32_t* GetStride() const { return (int32_t*)planarStride; };
	inline int64_t  GetPts() const { return frameDesc.ptsStart; };
	inline int32_t GetWidth() const { return frameDesc.width; };
	inline int32_t GetHeight() const { return frameDesc.height; };
	inline DWORD  GetPixelFormat() const{ return frameDesc.pixelFormatInFourCC; };
	inline int32_t GetPlanarCount() const { return planarCnt; };
	inline int32_t GetLineSize() const { return planarStride[0]; };

private:

	// descriptor this sample
	uint32_t capacity; // size of buffer 
	uint8_t *dataPtr;

	int32_t planarCnt;
	uint8_t *planarPtr[4];
	int32_t planarStride[4];
	int32_t planarSize[4];

	FRAME_DESC frameDesc;
};

