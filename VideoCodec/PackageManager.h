#pragma once
#include "common\PackageBuffer.h"
#include "common\AutoLock.h"
#include <list>
#include <stdint.h>

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

typedef std::list<CPackageBuffer*> PACKAGELIST;

class CPackageBufferManager
{
public:
	CPackageBufferManager();
	~CPackageBufferManager();

	BOOL Reset(int32_t);
	BOOL LockPackage(CPackageBuffer *&);
	BOOL UnlockPackage(CPackageBuffer *&);
	BOOL GetPackage(CPackageBuffer *&);

protected:
	BOOL AllocMemoryBySizeInByte(uint8_t* ptr, int32_t);
	BOOL ReleaseMemory(uint8_t *);
	BOOL ClearWorkStatus();

private:
	PACKAGELIST readyList;
	PACKAGELIST emptyList;
	PACKAGELIST occupyList;

	CLock mListLock;
};

