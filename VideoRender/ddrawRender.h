#pragma once

#include "SampleBuffer.h"
#include <timeApi.h>
#include <ddraw.h>
#include <stdint.h>
#include "PixelFormat.h"
#include "utils.h"

#include "SlidingWindowCalc.h"

class DDrawRender
{
public:
	DDrawRender();
	DDrawRender(HWND);
	~DDrawRender();

	HRESULT InitializeRenderContext(int, int, DWORD);
	HRESULT DeinitRenderContext();

	HRESULT PushFrame(CSampleBuffer *frame);
	DWORD RenderLoop();

protected:
	HRESULT CreateSurfaces(int, int, DWORD);
	const char* GetErrorString(HRESULT );
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

