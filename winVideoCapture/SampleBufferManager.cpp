#include "stdafx.h"
#include "SampleBufferManager.h"


CSampleBufferManager::CSampleBufferManager()
{
}


CSampleBufferManager::~CSampleBufferManager()
{
}

BOOL CSampleBufferManager::reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = FALSE;
	int32_t buffSizePreFrame = GetFrameSizeByRes(res);
	ReleaseMemory();
	bRet = AllocMemoryBySizeInByte(buffSizePreFrame * nbFrames);
	if (bRet){
		bufferList.resize(nbFrames);
	}
	BUFFLIST::iterator it = bufferList.begin();
	for (; it != bufferList.end(); it++){
		it->reset()
	}

	return bRet;
}

BOOL CSampleBufferManager::ReleaseMemory()
{
	bufferList.clear();
	emptyBufferList.clear();

	if (bufferPtr){
		_aligned_free(bufferPtr);
		bufferPtr = NULL;
	}

	bufferSize = 0;

	return TRUE;
}

BOOL CSampleBufferManager::AllocMemoryBySizeInByte(int32_t sizeInBytes)
{
	bufferPtr = (uint8_t*)_aligned_malloc(sizeInBytes, 32);
	if (!bufferPtr){
		bufferSize = sizeInBytes;
	}

	return !bufferPtr;
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