#pragma once

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
	ColorRange_Full = 1, // 0~255;0~1023,10bit;also define in PC
	ColorRange_Video = 2, // 16~235；64~940；also mean TV
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

struct VideoSampleBuffer
{
public:
	VideoSampleBuffer() { Reset(0, 0);  };

	~VideoSampleBuffer(){};

	/* for constructor, should first call */
	BOOL Reset(uint8_t *bufferPtr, int32_t capacityInBytes)
	{
		this->capacity = capacityInBytes;
		this->bufferPtr = bufferPtr;
		width = 0;
		height = 0;
		ptsStart = ptsEnd = 0;
		frameStartIdx = frameEndIdx = 0;
		pixelFormatInFourCC = PIXEL_FORMAT_NONE;
		planarCnt = 0;
		dataPtr = NULL;
		validDataSize = 0;
		ZeroMemory(resShift, sizeof(resShift));
		ZeroMemory(planarPtr, sizeof(planarPtr));
		ZeroMemory(planarSize, sizeof(planarSize));
		ZeroMemory(planarStride, sizeof(planarStride));
		colorRange = ColorRange_Full;
		colorTransfer = ColorTransfer_Unspecified;
		colorMatrix = ColorMatrix_Unspecified;
		colorPrimaries = ColorPrimaries_Unspecified;
		return TRUE;
	}
	
	/* for constructor */
	BOOL Reset(uint8_t *bufferPtr, int32_t capacityInBytes, int32_t width, int32_t height, DWORD pixelFormat)
	{
		Reset(bufferPtr, capacityInBytes);
		this->pixelFormatInFourCC = pixelFormat;
		this->width = width;
		this->height = height;
		this->bufferPtr = bufferPtr;
		this->capacity = capacityInBytes;
		this->dataPtr = bufferPtr;
		this->validDataSize = capacityInBytes;
		FillPixFmtDescInSampleBuffer(width, height);
		return TRUE;
	}

	BOOL FillData(VideoSampleBuffer &desc)
	{
		if (desc.validDataSize > this->capacity || !desc.dataPtr){
			return FALSE;
		}

		*this = desc;

		return TRUE;
	}

	VideoSampleBuffer & operator = (VideoSampleBuffer &desc)
	{
		this->width = desc.width;
		this->height = desc.height;

		this->ptsStart = desc.ptsStart;
		this->ptsEnd = desc.ptsEnd;
		this->frameStartIdx = desc.frameStartIdx;
		this->frameEndIdx = desc.frameEndIdx;

		this->pixelFormatInFourCC = desc.pixelFormatInFourCC;
		this->colorRange = desc.colorRange;
		this->colorTransfer = desc.colorTransfer;
		this->colorPrimaries = desc.colorPrimaries;
		this->colorMatrix = desc.colorMatrix;

		memcpy_s(this->bufferPtr, this->capacity, desc.dataPtr, desc.validDataSize);
		this->validDataSize = desc.validDataSize;

		FillPixFmtDescInSampleBuffer(desc.width, desc.height);

		return *this;
	}

	inline uint8_t *GetDataPtr() const { return bufferPtr; };

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

	int32_t planarCnt;
	uint8_t *planarPtr[4];
	int32_t planarStride[4];
	int32_t planarSize[4];
	struct {
		int32_t widthShift;
		int32_t heightShift;
	} resShift[4];

protected:

	BOOL FillPixFmtDescInSampleBuffer(int32_t width, int32_t height){
		const PIXELFORAMTDESC * info = GetPxielFormatDescByFourCC(this->pixelFormatInFourCC);

		planarCnt = info->pixdesc.planarCnt;

		planarStride[0] = (((width * info->pixdesc.bpp + info->strideW - 1)&~(info->strideW - 1)) >> 3) >> info->pixdesc.resShift[0].widthShift;
		planarSize[0] = planarStride[0] * (height >> info->pixdesc.resShift[0].heightShift);
		planarPtr[0] = bufferPtr;
		resShift[0].widthShift = info->pixdesc.resShift[0].widthShift;
		resShift[0].heightShift = info->pixdesc.resShift[0].heightShift;
		
		planarStride[1] = (((width * info->pixdesc.bpp + info->strideW - 1)&~(info->strideW - 1)) >> 3) >> info->pixdesc.resShift[1].widthShift;
		planarSize[1] = planarStride[1] * (height >> info->pixdesc.resShift[1].heightShift);
		planarPtr[1] = planarPtr[0] + planarSize[0];
		resShift[1].widthShift = info->pixdesc.resShift[1].widthShift;
		resShift[1].heightShift = info->pixdesc.resShift[1].heightShift;

		planarStride[2] = (((width * info->pixdesc.bpp + info->strideW - 1)&~(info->strideW - 1)) >> 3) >> info->pixdesc.resShift[2].widthShift;
		planarSize[2] = planarStride[2] * (height >> info->pixdesc.resShift[2].heightShift);
		planarPtr[2] = planarPtr[1] + planarSize[1];

		resShift[2].widthShift = info->pixdesc.resShift[2].widthShift;
		resShift[2].heightShift = info->pixdesc.resShift[2].heightShift;

		return TRUE;
	}

private:

	// descriptor this sample
	uint32_t capacity; // size of buffer 
	uint8_t *bufferPtr;
};

