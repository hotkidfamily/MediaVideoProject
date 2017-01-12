#pragma once
#include "IRender.h"

#include "SampleBuffer.h"
#include <timeApi.h>
#include <ddraw.h>
#include <stdint.h>
#include "PixelFormat.h"
#include "utils.h"
#include "RenderUtils.h"

#include "SlidingWindowCalc.h"

class DDrawRender :public IRender, public IRenderThread
{
public:
	DDrawRender();
	DDrawRender(HWND);
	~DDrawRender();

	BOOL InitRender(const RENDERCONFIG &);
	BOOL DeinitRender();
	BOOL Repaint() { return S_OK; };

	BOOL PushFrame(CSampleBuffer *);

	const char *GetRenderDescriptor() const { return "Ddraw Render"; };

	virtual DWORD RenderLoop();

protected:
	HRESULT CreateSurfaces(int, int, DWORD);
	void FillddPixelFormatFromFourCC(LPDDPIXELFORMAT ddPixelFormat, DWORD dwFourCC);
	BOOL OSDText(HDC, char *, ...);

private:
	IDirectDraw7 *mDDrawObj;
	LPDIRECTDRAWSURFACE7 mPrimarySurface;
	LPDIRECTDRAWSURFACE7 mCanvasSurface;
	LPDIRECTDRAWCLIPPER mDDrawClippper;
	DDCAPS mHwCaps, mHelCaps;

	HANDLE mRenderEvent;
	BOOL mSupportVSync;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;

	HWND mHwnd;
	BOOL mRenderThreadRuning;
	SIZE mScreenSizeInPixel;

	int64_t mLastPts;
	CSlidingWindowCalc mInputStatis;
	DWORD mLastTime;
	CSlidingWindowCalc mRenderStatis;
	int32_t mCanvasBpp;
};

