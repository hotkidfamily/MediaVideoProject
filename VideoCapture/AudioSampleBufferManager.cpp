#include "stdafx.h"
#include "AudioSampleBufferManager.h"


AudioSampleBufferManager::AudioSampleBufferManager()
	: mBufferSize(0)
{
	readyList.clear();
	emptyList.clear();
	occupyList.clear();
	ZeroMemory(mBufferPtr, sizeof(mBufferPtr));
	InitializeCriticalSection(&mCs);
}

AudioSampleBufferManager::~AudioSampleBufferManager()
{
	ClearWorkStatus();

	for (int i = 0; i < mNbBuffers; i++){
		DeallocMemory(mBufferPtr[i]);
		mBufferPtr[i] = nullptr;
	}

	DeleteCriticalSection(&mCs);
}

BOOL AudioSampleBufferManager::Reset(int32_t res, int32_t nbFrames)
{
	BOOL bRet = TRUE;
	int32_t index = 0;
	int32_t buffSizePreFrame = 1 * 1024 * 1024;
	ABUFFLIST::iterator it;

	ClearWorkStatus();

	emptyList.resize(nbFrames > MAX_AUDIO_FRAME_SIZE ? MAX_AUDIO_FRAME_SIZE : nbFrames);
	
	int i = 0;
	for (it = emptyList.begin(); it != emptyList.end(); it++){
		*it = new AudioSampleBuffer;
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

QUEUE_RET AudioSampleBufferManager::FillFrame(AudioSampleBuffer &desc)
{
	QUEUE_RET ret = Q_SUCCESS;
	CAutoLock lock(mCs);

	if (!emptyList.empty()){
		AudioSampleBuffer *sample = emptyList.front();
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

BOOL AudioSampleBufferManager::LockFrame(AudioSampleBuffer *&buf)
{
	BOOL bRet = FALSE;
	CAutoLock lock(mCs);

	if (!readyList.empty()){
		AudioSampleBuffer *sample = readyList.front();
		readyList.pop_front();
		occupyList.push_back(sample);
		buf = sample;
		bRet = TRUE;
	}

	return bRet;
}

BOOL AudioSampleBufferManager::UnlockFrame(AudioSampleBuffer *&sample)
{
	BOOL bRet = TRUE;
	CAutoLock lock(mCs);

	if (sample){
		occupyList.remove(sample);
		emptyList.push_back(sample);
	}

	return bRet;
}

BOOL AudioSampleBufferManager::ClearWorkStatus()
{
	if (!readyList.empty()){
		ABUFFLIST::iterator it = readyList.begin();
		for (; it != readyList.end(); it++){
			delete *it;
		}
	}

	if (!emptyList.empty()){
		ABUFFLIST::iterator it = emptyList.begin();
		for (; it != emptyList.end(); it++){
			delete *it;
		}
	}

	if (!occupyList.empty()){
		ABUFFLIST::iterator it = occupyList.begin();
		for (; it != occupyList.end(); it++){
			delete *it;
		}
	}

	readyList.clear();
	emptyList.clear();
	occupyList.clear();

	return TRUE;
}
