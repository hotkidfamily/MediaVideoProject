#pragma once

#include "SampleBuffer.h"
#include <timeApi.h>
#include <ddraw.h>

#include "SlidingWindowCalc.h"

#ifndef CHECK_HR
#define CHECK_HR(hr) if (FAILED(hr)) { goto done; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if(x) x->Release(); x=NULL; }
#endif

class DDrawRender
{
public:
	DDrawRender();
	DDrawRender(HWND);
	~DDrawRender();

	HRESULT InitDDrawInterface(int, int, DWORD);
	HRESULT DeinitDDrawInterface();

	HRESULT PushFrame(CSampleBuffer *frame);
	DWORD RenderLoop();

protected:
	HRESULT CreateSurfaces(int, int, DWORD);
	const char* GetDDrawErrorString(HRESULT );
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

