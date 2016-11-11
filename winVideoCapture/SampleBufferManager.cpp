#include "stdafx.h"
#include "SampleBufferManager.h"


CSampleBufferManager::CSampleBufferManager()
	: mBufferSize(0)
	, mBufferPtr(0)
{
}

CSampleBufferManager::~CSampleBufferManager()
{
	ReleaseMemory();
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

	readyList.resize(nbFrames);
	for (it = readyList.begin(); it != readyList.end(); it++){
		it->Reset(mBufferPtr + index*buffSizePreFrame, buffSizePreFrame);
	}

	bRet = TRUE;

errRet:
	return bRet;
}

BOOL CSampleBufferManager::FillOneFrame(uint8_t* data, int32_t dataSize, int64_t pts, int32_t pixelFormat)
{
	BOOL bRet = FALSE;

	if (emptyList.size()){
		CSampleBuffer &sample = emptyList.front();
		bRet = sample.FillData(data, dataSize, pts, pixelFormat);
		if (bRet){
			emptyList.pop_front();
			readyList.push_back(sample);
		}
	}

	return bRet;
}

BOOL CSampleBufferManager::LockFrame(CSampleBuffer *buf)
{
	BOOL bRet = FALSE;

	if (readyList.size()){
		CSampleBuffer &sample = readyList.front();
		buf = &sample;
		occupyList.push_back(sample);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSampleBufferManager::UnlockFrame(CSampleBuffer *buf)
{
	BOOL bRet = TRUE;

	if (buf){
		emptyList.push_back(*buf);

		BUFFLIST::iterator it = occupyList.begin();
		for (; it != occupyList.end(); it++){
			if ((int32_t)(it->GetDataPtr()) == (int32_t)(*buf->GetDataPtr())){
				break;
			}
		}
		occupyList.erase(it);
	}

	return bRet;
}

BOOL CSampleBufferManager::ReleaseMemory()
{
	readyList.clear();
	emptyList.clear();

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