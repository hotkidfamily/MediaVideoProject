#include "stdafx.h"
#include "SampleBufferManager.h"


VideoSampleBufferManager::VideoSampleBufferManager()
	: mBufferSize(0)
	, mBufferPtr(nullptr)
{
	readyList.clear();
	emptyList.clear();
	occupyList.clear();
	InitializeCriticalSection(&mCs);
}

VideoSampleBufferManager::~VideoSampleBufferManager()
{
	ClearWorkStatus();
	DeallocMemory(mBufferPtr);
	mBufferPtr = nullptr;
	DeleteCriticalSection(&mCs);
}

BOOL VideoSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = FALSE;
	int32_t index = 0;
	int32_t buffSizePreFrame = GetFrameSizeByRes(res);
	int32_t buffSize = buffSizePreFrame * nbFrames;
	BUFFLIST::iterator it;

	ClearWorkStatus();

	// low than current buffer
	if (buffSizePreFrame * nbFrames > mBufferSize){

		bRet = ReallocMemory(&mBufferPtr, buffSize);

		if (!bRet){
			// out of memory
			goto errRet;
		} else{
			mBufferSize = buffSize;
		}
	}

	emptyList.resize(nbFrames);
	for (it = emptyList.begin(); it != emptyList.end(); it++){
		*it = new VideoSampleBuffer;
		(*it)->Reset(mBufferPtr + index*buffSizePreFrame, buffSizePreFrame);
	}

	bRet = TRUE;
	logger(Info, "Create %d frames queue.\n", nbFrames);
	mNbBuffers = nbFrames;

errRet:
	return bRet;
}

QUEUE_RET VideoSampleBufferManager::FillFrame(VideoSampleBuffer &desc)
{
	QUEUE_RET ret = Q_SUCCESS;
	CAutoLock lock(mCs);

	if (!emptyList.empty()){
		VideoSampleBuffer *sample = emptyList.front();
		if (sample->FillData(desc)){
			// FixME£º
			emptyList.pop_front();
			readyList.push_back(sample);
		} else{
			ret = Q_NOMEMORY;
		}
	} else{
		ret = Q_FULL;
	}

	return ret;
}

BOOL VideoSampleBufferManager::LockFrame(VideoSampleBuffer *&buf)
{
	BOOL bRet = FALSE;
	CAutoLock lock(mCs);

	if (!readyList.empty()){
		VideoSampleBuffer *sample = readyList.front();
		readyList.pop_front();
		occupyList.push_back(sample);
		buf = sample;
		bRet = TRUE;
	}

	return bRet;
}

BOOL VideoSampleBufferManager::UnlockFrame(VideoSampleBuffer *&sample)
{
	BOOL bRet = TRUE;
	CAutoLock lock(mCs);

	if (sample){
		occupyList.remove(sample);
		emptyList.push_back(sample);
	}

	return bRet;
}

BOOL VideoSampleBufferManager::ClearWorkStatus()
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

int32_t VideoSampleBufferManager::GetFrameSizeByRes(int32_t res)
{
	int32_t sizePrePlanner = 0;
	
	sizePrePlanner = GetFrameSizePrePlannerByRes((E_RES)res);

	return sizePrePlanner * 4; /*ALLOC MEMORY IN MAX PLANNER CURRENT MAX PLANNER = 4*/
}
