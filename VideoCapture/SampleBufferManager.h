#pragma once

typedef enum tagQUEUE_ERROR {
	Q_NOMEMORY = -2,
	Q_FULL = -1,
	Q_SUCCESS = 0,
}QUEUE_RET;

typedef std::list<VideoSampleBuffer*> BUFFLIST;
#define MAX_FRAME_SIZE (20)

class VideoSampleBufferManager
{
public:
	VideoSampleBufferManager();
	~VideoSampleBufferManager();
	
	BOOL Reset(int32_t resOfFrames, int32_t nbFrames);
	BOOL LockFrame(VideoSampleBuffer *&);
	BOOL UnlockFrame(VideoSampleBuffer *&);
	QUEUE_RET FillFrame(VideoSampleBuffer &);

protected:
	int32_t GetFrameSizeByRes(int32_t resOfFrames);
	BOOL ClearWorkStatus();

private:
	BUFFLIST readyList;
	BUFFLIST emptyList;
	BUFFLIST occupyList;

	int32_t mNbBuffers;

	uint8_t *mBufferPtr[MAX_FRAME_SIZE];
	int32_t mBufferSize;

	CRITICAL_SECTION mCs;
};

