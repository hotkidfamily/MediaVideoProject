#pragma once

#include "SampleBuffer.h"
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

protected:
	HRESULT CreateSurfaces(int, int, DWORD);
	const char* GetDDrawErrorString(HRESULT );
	DWORD RenderLoop();
	void FillddPixelFormatFromFourCC(LPDDPIXELFORMAT ddPixelFormat, DWORD dwFourCC);
	BOOL OSDText(HDC, char *, ...);

private:
	IDirectDraw7 *mDDrawObj;
	LPDIRECTDRAWSURFACE7 mPrimarySurface;
	LPDIRECTDRAWSURFACE7 mCanvasSurface;
	LPDIRECTDRAWCLIPPER mDDrawClippper;
	DDCAPS mHwCaps, mHelCaps;

	HANDLE mRenderEvent;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;

	HWND mHwnd;
	BOOL bRender;

	int64_t mLastPts;
	CSlidingWindowCalc mRenderStatis;
};

