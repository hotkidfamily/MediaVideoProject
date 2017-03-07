#pragma once

#define MAX_RENDER_OBJ (5)

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
	BOOL OSDText(HDC, RECT *, TCHAR *format, ...);
	BOOL DrawStatus();
	BOOL UpdatePushStatis(CSampleBuffer *&);
	BOOL UpdateRenderStatis();

private:
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

