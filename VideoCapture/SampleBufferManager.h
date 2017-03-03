#pragma once

typedef enum tagQUEUE_ERROR {
	Q_NOMEMORY = -2,
	Q_FULL = -1,
	Q_SUCCESS = 0,
}QUEUE_RET;

typedef std::list<CSampleBuffer*> BUFFLIST;

class CSampleBufferManager
{
public:
	CSampleBufferManager();
	~CSampleBufferManager();
	
	BOOL Reset(int32_t resOfFrames, int32_t nbFrames);
	BOOL LockFrame(CSampleBuffer *&);
	BOOL UnlockFrame(CSampleBuffer *&);
	QUEUE_RET FillFrame(FRAME_DESC &);

protected:
	int32_t GetFrameSizeByRes(int32_t resOfFrames);
	BOOL ClearWorkStatus();

private:
	BUFFLIST readyList;
	BUFFLIST emptyList;
	BUFFLIST occupyList;

	int32_t mNbBuffers;

	uint8_t *mBufferPtr;
	int64_t mBufferSize;

	CRITICAL_SECTION mCs;
};

