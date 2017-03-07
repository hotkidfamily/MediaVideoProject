#pragma once

#define MAX_RENDER_OBJ (5)

class D3D9Render : public IRender, public IRenderThread
{
public:
	D3D9Render();
	~D3D9Render();

	virtual BOOL InitRender(const RENDERCONFIG &);
	virtual BOOL DeinitRender();

	virtual BOOL PushFrame(VideoSampleBuffer *);
	virtual const char *GetRenderDescriptor() const { return "D3D9 sprite render"; };

	virtual DWORD RenderLoop();

protected:
	HRESULT GetDeviceType(D3DDISPLAYMODE);
	HRESULT IfSupportedFormat(D3DDISPLAYMODE , D3DFORMAT );
	BOOL IfSupportedConversionFormat(D3DDISPLAYMODE , D3DFORMAT);
	HRESULT UpdateRenderSurface(VideoSampleBuffer *&);
	BOOL OSDText(HDC, RECT *, TCHAR *format, ...);
	BOOL DrawStatus();
	BOOL UpdatePushStatis(VideoSampleBuffer *&);
	BOOL UpdateRenderStatis();

private:
	HWND mhWnd;

	TCHAR *m_backModeDesc;
	TCHAR *m_mainModeDesc;
	IDirect3D9Ex* mpD3D9OBj;
	IDirect3DDevice9Ex* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	BOOL mbSupportConversion;
	RENDERCONFIG mConfig;

	enum SSurfaceType {
		SUPPORT_TEXTURE,
		SUPPORT_SURFACE
	};

	SSurfaceType mSupportSurfaceType;
	int32_t mCurPushObjIndex;
	int32_t mCurRenderObjIndex;
	LPDIRECT3DTEXTURE9 mpD3D9Texture[MAX_RENDER_OBJ];
	LPDIRECT3DSURFACE9 mpD3D9Surface[MAX_RENDER_OBJ];

	LPD3DXFONT mPFont;

	HANDLE mRenderEvent;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;

	// VPP 
	BOOL mbNeedVpp;
	IVPPFactory *mVppFactory;
	IVPP *mVpp;
	IVPPPARAMETER vppParams;
	VideoSampleBuffer *mVppTransSampleBuffer;

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

