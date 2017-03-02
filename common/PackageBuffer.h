#pragma once

#include "utils.h"

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

enum FRAME_TYPE{
	ERR_FRAME = 0,
	IDR_FRAME = 1,
	P_FRAME = 2,
	I_FRAME = 3,
	B_FRAME = 4
};

class CPackageBuffer
{
public:
	CPackageBuffer()
		: mBufferPtr(nullptr)
		, mCapbility(0)
		, data(nullptr)
		, extraData(nullptr)
		, dataSize(0)
		, extraDataSize(0)
		, pts(0)
		, dts(0)
		, frameType(ERR_FRAME)
		, extraParam(nullptr)
	{
	}

	CPackageBuffer(int32_t size, int32_t mainSize)
		: mBufferPtr(nullptr)
		, mCapbility(0)
		, data(nullptr)
		, extraData(nullptr)
		, dataSize(0)
		, extraDataSize(0)
		, pts(0)
		, dts(0)
		, frameType(ERR_FRAME)
		, extraParam(nullptr)
	{
		Reset(size, mainSize);
	}

	~CPackageBuffer(){
		DeallocMemory(mBufferPtr);
		mBufferPtr = 0;
	}

	inline bool isIDRFrame() const { return frameType == IDR_FRAME; }
	inline bool isIFrame() const{ return frameType == I_FRAME; }
	inline bool isBFrame() const { return frameType == B_FRAME; }
	inline bool isPFrame() const { return frameType == P_FRAME; }
	inline bool isErrFrame() const { return frameType == ERR_FRAME; }

	inline uint8_t* Data() const { return data; }
	inline uint32_t DataSize() const { return dataSize; }
	inline uint8_t* ExtraData() const { return extraData; }
	inline uint32_t ExtraDataSize() const { return extraDataSize; }
	inline int64_t Pts() const { return pts; };
	inline int64_t Dts() const { return dts; };
	inline uint32_t StreamType() const { return streamType; };

	inline void SetPts(int64_t pts) { this->pts = pts; };
	inline void SetDts(int64_t dts) { this->dts = dts; };
	inline void SetFrameType(FRAME_TYPE type) { this->frameType = type; };

	BOOL Reset(uint32_t totalSize, uint32_t mainDataSize) {
		BOOL bRet = FALSE;
		if (mainDataSize > totalSize){
			goto done;
		}

		if (mCapbility < totalSize){
			// realloc
			if (!ReallocMemory(&mBufferPtr, totalSize)){
				goto done;
			}
		}
		
		data = mBufferPtr;
		dataSize = mainDataSize;
		extraData = mBufferPtr + mainDataSize;
		extraDataSize = totalSize - mainDataSize;
		pts = 0;
		dts = 0;
		frameType = ERR_FRAME;
		extraParam = nullptr;
		bRet = TRUE;

	done:
		return bRet;
	}

protected:
	FRAME_TYPE frameType;
	int64_t pts;
	int64_t dts; /* sometimes dts is negative value */
	uint8_t *data;
	uint32_t dataSize;
	uint8_t *extraData;
	uint32_t extraDataSize;
	uint32_t streamType; /* avc hevc or etc... */

	// reserved for future
	void *extraParam;

private:
	uint32_t mCapbility;
	uint8_t *mBufferPtr;
};
