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

	HRESULT InitDDrawInterface(int, int);
	HRESULT DeinitDDrawInterface();

	HRESULT PushFrame(CSampleBuffer *frame);

protected:
	HRESULT CreateSurfaces(int ,int );
	const char* GetDDrawErrorString(HRESULT );

private:
	IDirectDraw *mDDrawObj;
	LPDIRECTDRAWSURFACE mDDrawPrimarySurface;
	LPDIRECTDRAWSURFACE mDDrawSecondarySurface;
	HWND mHwnd;

	DWORD mWidth;
	DWORD mHeight;
};

