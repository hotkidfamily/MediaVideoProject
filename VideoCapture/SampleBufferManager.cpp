#include "stdafx.h"
#include "SampleBufferManager.h"


CSampleBufferManager::CSampleBufferManager()
	: mBufferSize(0)
	, mBufferPtr(nullptr)
{
	readyList.clear();
	emptyList.clear();
	occupyList.clear();
	InitializeCriticalSection(&mCs);
}

CSampleBufferManager::~CSampleBufferManager()
{
	ReleaseMemory();
	DeleteCriticalSection(&mCs);
}

BOOL CSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = FALSE;
	int32_t index = 0;
	int32_t buffSizePreFrame = GetFrameSizeByRes(res);
	BUFFLIST::iterator it;

	ClearWorkStatus();

	// low than current buffer
	if (buffSizePreFrame * nbFrames > mBufferSize){
		ReleaseMemory();
		bRet = AllocMemoryBySizeInByte(buffSizePreFrame * nbFrames);
		if (!bRet){
			// out of memory
			goto errRet;
		}
	}

	emptyList.resize(nbFrames);
	for (it = emptyList.begin(); it != emptyList.end(); it++){
		*it = new CSampleBuffer(mBufferPtr + index*buffSizePreFrame, buffSizePreFrame);
	}

	bRet = TRUE;

errRet:
	return bRet;
}

BOOL CSampleBufferManager::FillFrame(FRAME_DESC &desc)
{
	BOOL bRet = FALSE;
	CAutoLock lock(mCs);

	if (!emptyList.empty()){
		CSampleBuffer *sample = emptyList.front();
		bRet = sample->FillData(desc);
		if (bRet){
			// FixME£º
			emptyList.pop_front();
			readyList.push_back(sample);
		}
	}

	if(!bRet){
		// drop one frame
	}

	return bRet;
}

BOOL CSampleBufferManager::LockFrame(CSampleBuffer *&buf)
{
	BOOL bRet = FALSE;
	CAutoLock lock(mCs);

	if (!readyList.empty()){
		CSampleBuffer *sample = readyList.front();
		readyList.pop_front();
		occupyList.push_back(sample);
		buf = sample;
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSampleBufferManager::UnlockFrame(CSampleBuffer *&sample)
{
	BOOL bRet = TRUE;
	CAutoLock lock(mCs);

	if (sample){
		occupyList.remove(sample);
		emptyList.push_back(sample);
	}

	return bRet;
}

BOOL CSampleBufferManager::ReleaseMemory()
{
	ClearWorkStatus();

	if (mBufferPtr){
		DeallocMemory(mBufferPtr);
		mBufferPtr = nullptr;
	}

	mBufferSize = 0;

	return TRUE;
}

BOOL CSampleBufferManager::ClearWorkStatus()
{
	if (!readyList.empty()){
		BUFFLIST::iterator it = readyList.begin();
		for (; it != readyList.end(); it++){
			delete *it;
		}
	}

	if (!emptyList.empty()){
		BUFFLIST::iterator it = emptyList.begin();
		for (; it != emptyList.end(); it++){
			delete *it;
		}
	}

	if (!occupyList.empty()){
		BUFFLIST::iterator it = occupyList.begin();
		for (; it != occupyList.end(); it++){
			delete *it;
		}
	}

	readyList.clear();
	emptyList.clear();
	occupyList.clear();

	return TRUE;
}

BOOL CSampleBufferManager::AllocMemoryBySizeInByte(int32_t sizeInBytes)
{
	mBufferPtr = (uint8_t*)AllocMemory(sizeInBytes);
	if (mBufferPtr){
		mBufferSize = sizeInBytes;
	}

	return !!mBufferPtr;
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
		width = WIDTHALIGN(1920);
		height = HEIGHTALIGN(1080);
		break;
	case RES720P:
		width = WIDTHALIGN(1280);
		height = HEIGHTALIGN(720);
		break;
	case RES4K:
		width = WIDTHALIGN(3840);
		height = HEIGHTALIGN(2160);
		break;
	case RES8K:
		width = WIDTHALIGN(7680);
		height = HEIGHTALIGN(4320);
		break;
	default:
		break;
	}

	sizePrePlanner = width * height;

	return sizePrePlanner;
}