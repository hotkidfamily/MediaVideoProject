#pragma once

typedef std::list<AudioSampleBuffer*> ABUFFLIST;
#define MAX_AUDIO_FRAME_SIZE (20)

class AudioSampleBufferManager
{
public:
	AudioSampleBufferManager();
	~AudioSampleBufferManager();
	
	BOOL Reset(int32_t resOfFrames, int32_t nbFrames);
	BOOL LockFrame(AudioSampleBuffer *&);
	BOOL UnlockFrame(AudioSampleBuffer *&);
	QUEUE_RET FillFrame(AudioSampleBuffer &);

protected:
	int32_t GetFrameSizeByRes(int32_t resOfFrames);
	BOOL ClearWorkStatus();

private:
	ABUFFLIST readyList;
	ABUFFLIST emptyList;
	ABUFFLIST occupyList;

	int32_t mNbBuffers;

	uint8_t *mBufferPtr[MAX_AUDIO_FRAME_SIZE];
	int32_t mBufferSize;

	CRITICAL_SECTION mCs;
};

