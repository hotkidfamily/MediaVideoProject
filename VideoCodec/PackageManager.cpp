#include "stdafx.h"
#include "PackageManager.h"

CPackageBufferManager::CPackageBufferManager()
{

}

CPackageBufferManager::~CPackageBufferManager()
{
	ClearWorkStatus();
}

BOOL CPackageBufferManager::Reset(int32_t nbFrames)
{
	BOOL bRet = FALSE;
	int32_t index = 0;
	PACKAGELIST::iterator it;
	mListLock.Lock();

	ClearWorkStatus();

	// low than current buffer
	if (nbFrames > 0){
		emptyList.resize(nbFrames);
		for (it = emptyList.begin(); it != emptyList.end(); it++){
			*it = new CPackageBuffer(2*1024*1024, 5*1024*1024/3);
		}
	}

	bRet = TRUE;

	mListLock.Unlock();
	return bRet;
}

BOOL CPackageBufferManager::GetPackage(CPackageBuffer *&buf)
{
	mListLock.Lock();
	if (emptyList.size()){
		buf = emptyList.front();
		emptyList.pop_front();
		readyList.push_back(buf);
	}

	return TRUE;
}

BOOL CPackageBufferManager::LockPackage(CPackageBuffer *&buf)
{
	BOOL bRet = FALSE;
	mListLock.Lock();

	if (readyList.size()){
		CPackageBuffer *sample = readyList.front();
		buf = sample;
		occupyList.push_back(sample);
		readyList.pop_front();
		bRet = TRUE;
	}

	mListLock.Unlock();
	return bRet;
}

BOOL CPackageBufferManager::UnlockPackage(CPackageBuffer *&sample)
{
	BOOL bRet = TRUE;
	mListLock.Lock();

	if (sample){
		emptyList.push_back(sample);
		occupyList.remove(sample);
	}

	mListLock.Unlock();
	return bRet;
}

BOOL CPackageBufferManager::ClearWorkStatus()
{
	if (readyList.size()){
		PACKAGELIST::iterator it = readyList.begin();
		for (; it != readyList.end(); it++){
			delete *it;
		}
	}

	if (emptyList.size()){
		PACKAGELIST::iterator it = emptyList.begin();
		for (; it != emptyList.end(); it++){
			delete *it;
		}
	}

	if (occupyList.size()){
		PACKAGELIST::iterator it = occupyList.begin();
		for (; it != occupyList.end(); it++){
			delete *it;
		}
	}

	readyList.clear();
	emptyList.clear();
	occupyList.clear();

	return TRUE;
}
