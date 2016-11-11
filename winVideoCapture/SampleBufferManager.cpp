#include "stdafx.h"
#include "SampleBufferManager.h"


CSampleBufferManager::CSampleBufferManager()
	: mBufferSize(0)
	, mBufferPtr(0)
{
}


CSampleBufferManager::~CSampleBufferManager()
{
}

BOOL CSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = FALSE;
	int32_t index = 0;
	int32_t buffSizePreFrame = GetFrameSizeByRes(res) * nbFrames;
	BUFFLIST::iterator it;

	// low than current buffer
	if (buffSizePreFrame * nbFrames > mBufferSize){
		ReleaseMemory();
		bRet = AllocMemoryBySizeInByte(buffSizePreFrame * nbFrames);
		if (!bRet){
			// out of memory
			goto errRet;
		}
	}

	readyBufferList.resize(nbFrames);
	for (it = readyBufferList.begin(); it != readyBufferList.end(); it++){
		it->Reset(mBufferPtr + index*buffSizePreFrame, buffSizePreFrame);
	}

	bRet = TRUE;

errRet:
	return bRet;
}


BOOL CSampleBufferManager::FillOneFrame(uint8_t* data, int32_t dataSize, int64_t pts, int32_t pixelFormat)
{
	BOOL bRet = FALSE;

	if (emptyBufferList.size()){
		CSampleBuffer &sample = emptyBufferList.front();
		bRet = sample.FillData(data, dataSize, pts, pixelFormat);
		if (bRet){
			emptyBufferList.pop_front();
			readyBufferList.push_back(sample);
		}
	}
	return bRet;
}

BOOL CSampleBufferManager::LockFrame(CSampleBuffer *buf)
{
	BOOL bRet = FALSE;

	if (readyBufferList.size()){
		CSampleBuffer &sample = readyBufferList.front();
		buf = &sample;
		occupyBufferList.push_back(sample);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSampleBufferManager::UnlockFrame(CSampleBuffer *buf)
{
	BOOL bRet = TRUE;

	emptyBufferList.push_back(*buf);
	//occupyBufferList.remove(buf);

	return bRet;
}

BOOL CSampleBufferManager::ReleaseMemory()
{
	readyBufferList.clear();
	emptyBufferList.clear();

	if (mBufferPtr){
		_aligned_free(mBufferPtr);
		mBufferPtr = NULL;
	}

	mBufferSize = 0;

	return TRUE;
}

BOOL CSampleBufferManager::AllocMemoryBySizeInByte(int32_t sizeInBytes)
{
	mBufferPtr = (uint8_t*)_aligned_malloc(sizeInBytes, 32);
	if (!mBufferPtr){
		mBufferSize = sizeInBytes;
	}

	return !mBufferPtr;
}

int32_t CSampleBufferManager::GetFrameSizeByRes(int32_t res)
{
	int32_t sizePrePlanner = 0;
	
	sizePrePlanner = GetFrameSizePrePlannerByRes(res);

	return sizePrePlanner * 3;
}

int32_t CSampleBufferManager::GetFrameSizePrePlannerByRes(int32_t res)
{
	int32_t sizePrePlanner = 0;
	int32_t width = 0;
	int32_t height = 0;

	switch (res){
	case RES1080P:
		width = ALIGN32(1920);
		height = ALIGN32(1080);
		break;
	case RES720P:
		width = ALIGN32(1280);
		height = ALIGN32(720);
		break;
	case RES4K:
		width = ALIGN32(3840);
		height = ALIGN32(2160);
		break;
	case RES8K:
		width = ALIGN32(7680);
		height = ALIGN32(4320);
		break;
	default:
		break;
	}

	sizePrePlanner = width * height;

	return sizePrePlanner;
}