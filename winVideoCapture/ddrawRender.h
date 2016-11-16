#pragma once

#include "SampleBuffer.h"
#include <ddraw.h>

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

private:
	IDirectDraw7 *mDDrawObj;
	LPDIRECTDRAWSURFACE7 mDDrawPrimarySurface;
	LPDIRECTDRAWSURFACE7 mDDrawSecondarySurface;
	LPDIRECTDRAWCLIPPER mDDrawClippper;

	HANDLE mRenderEvent;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;

	HWND mHwnd;
	BOOL bRender;

	int64_t mLastPts;
};

