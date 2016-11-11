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

#define ALIGN32(x) ((x + 0x1F) & 0x20)


typedef std::list<CSampleBuffer> BUFFLIST;

class CSampleBufferManager
{
public:
	CSampleBufferManager();
	~CSampleBufferManager();
	
	BOOL reset(int32_t res, int32_t count);

protected:
	BOOL AllocMemoryBySizeInByte(int32_t);
	BOOL ReleaseMemory();
	int32_t GetFrameSizeByRes(int32_t res);
	int32_t GetFrameSizePrePlannerByRes(int32_t res);

private:
	BUFFLIST bufferList;
	BUFFLIST emptyBufferList;

	uint8_t *bufferPtr;
	int64_t bufferSize;
};

