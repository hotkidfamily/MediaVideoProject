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
	ClearWorkStatus();
	if (mBufferPtr)
		DeallocMemory(mBufferPtr);
	mBufferPtr = nullptr;
	DeleteCriticalSection(&mCs);
}

BOOL CSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
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
		*it = new CSampleBuffer(mBufferPtr + index*buffSizePreFrame, buffSizePreFrame);
	}

	bRet = TRUE;
	logger(Info, "Create %d frames queue.\n", nbFrames);
	mNbBuffers = nbFrames;

errRet:
	return bRet;
}

QUEUE_RET CSampleBufferManager::FillFrame(FRAME_DESC &desc)
{
	QUEUE_RET ret = Q_SUCCESS;
	CAutoLock lock(mCs);

	if (!emptyList.empty()){
		CSampleBuffer *sample = emptyList.front();
		if (sample->FillData(desc)){
			// FixME��
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

BOOL CSampleBufferManager::ClearWorkStatus()
{
	CAutoLock lock(mCs);

	readyList.clear();
	emptyList.clear();
	occupyList.clear();

	return TRUE;
}

int32_t CSampleBufferManager::GetFrameSizeByRes(int32_t res)
{
	// TODO: ����BPP�����ڴ棬���ﰴRGB32����

	int32_t sizePrePlanner = 0;
	
	sizePrePlanner = GetFrameSizePrePlannerByRes((E_RES)res);

	return sizePrePlanner * 4;
}
