#pragma once

#include <list>
#include "SampleBuffer.h"

enum resList{
	RES320P,
	RES480P,
	RES600P,
	RES768P,
	RES720P,
	RES1080P,
	RES4K,
	RES8K
};

#define ALIGN32(x) ((x + 0x1F) & (~0x1F))

typedef std::list<CSampleBuffer*> BUFFLIST;

class CSampleBufferManager
{
public:
	CSampleBufferManager();
	~CSampleBufferManager();
	
	BOOL Reset(int32_t resOfFrames, int32_t nbFrames);
	BOOL LockFrame(CSampleBuffer *&);
	BOOL UnlockFrame(CSampleBuffer *&);
	BOOL FillFrame(FRAME_DESC);

protected:
	BOOL AllocMemoryBySizeInByte(int32_t);
	BOOL ReleaseMemory();
	int32_t GetFrameSizeByRes(int32_t resOfFrames);
	int32_t GetFrameSizePrePlannerByRes(int32_t resOfFrames);
	BOOL ClearWorkStatus();

private:
	BUFFLIST readyList;
	BUFFLIST emptyList;
	BUFFLIST occupyList;

	uint8_t *mBufferPtr;
	int64_t mBufferSize;

	CLock mListLock;
};

