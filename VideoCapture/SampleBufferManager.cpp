#include "stdafx.h"
#include "SampleBufferManager.h"


VideoSampleBufferManager::VideoSampleBufferManager()
	: mBufferSize(0)
{
	readyList.clear();
	emptyList.clear();
	occupyList.clear();
	ZeroMemory(mBufferPtr, sizeof(mBufferPtr));
	InitializeCriticalSection(&mCs);
}

VideoSampleBufferManager::~VideoSampleBufferManager()
{
	ClearWorkStatus();

	for (int i = 0; i < mNbBuffers; i++){
		DeallocMemory(mBufferPtr[i]);
		mBufferPtr[i] = nullptr;
	}

	DeleteCriticalSection(&mCs);
}

BOOL VideoSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = TRUE;
	int32_t index = 0;
	int32_t buffSizePreFrame = GetFrameSizeByRes(res);
	BUFFLIST::iterator it;

	ClearWorkStatus();

	emptyList.resize(nbFrames);
	int i = 0;
	for (it = emptyList.begin(); it != emptyList.end(); it++){
		*it = new VideoSampleBuffer;
		bRet = ReallocMemory(&(mBufferPtr[i]), buffSizePreFrame);
		if (bRet){
			(*it)->Reset(mBufferPtr[i++] + index*buffSizePreFrame, buffSizePreFrame);
			mNbBuffers = i;
		} else{
			bRet = FALSE;
			break;
		}
	}

	logger(Info, "Create %d frames queue.\n", nbFrames);

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
