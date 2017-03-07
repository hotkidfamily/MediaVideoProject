#pragma once

#define MAX_AUDIO_BUF (5)
#define BUFFERNOTIFYSIZE (2048)

class D3DSoundRender : public IAudioRender
{
public:
	D3DSoundRender();
	~D3DSoundRender();

	virtual BOOL InitRender(const AudioRenderConfig &);
	virtual BOOL DeinitRender();

	virtual BOOL PushFrame(CSampleBuffer *);
	virtual const char *GetRenderDescriptor() const { return "D3D8sound render"; };

	virtual DWORD RenderLoop();

protected:
	HRESULT UpdateRenderSurface(CSampleBuffer *&);
	BOOL DrawStatus();
	BOOL UpdatePushStatis(CSampleBuffer *&);
	BOOL UpdateRenderStatis();

private:

	IDirectSound8 *m_pDS = NULL;
	IDirectSoundBuffer8 *m_pDSBuffer8 = NULL;	//used to manage sound buffers.
	IDirectSoundBuffer *m_pDSBuffer = NULL;
	IDirectSoundNotify8 *m_pDSNotify = NULL;
	DSBPOSITIONNOTIFY m_pDSPosNotify[MAX_AUDIO_BUF];
	HANDLE m_event[MAX_AUDIO_BUF];

	HANDLE mRenderEvent;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;

	// statistics 
	int64_t mLastPts;
	DWORD mLastRender;
	int32_t mCurRenderInterval;
	int64_t mCurPtsInterval;
	int64_t mDropFrameCount;
	CSlidingWindowCalc mInputStatis;
	CSlidingWindowCalc mRenderStatis;

	CSyncRender mRenderClock;
	BOOL mFirstRender;
};

