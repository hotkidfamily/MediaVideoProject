#include "stdafx.h"
#include "ddrawRender.h"


DDrawRender::DDrawRender()
	: mDDrawObj(NULL)
	, mDDrawPrimarySurface(NULL)
	, mDDrawSecondarySurface(NULL)
	, mHwnd(NULL)
	, mWidth(0)
	, mHeight(0)
{
}

DDrawRender::~DDrawRender()
{
}

HRESULT DDrawRender::CreateSurfaces(int width, int height)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC desc;
	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	desc.dwWidth = width;
	desc.dwHeight = height;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawPrimarySurface, NULL));
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawSecondarySurface, NULL));

done:
	return hr;
}

HRESULT DDrawRender::InitDDrawInterface(int width, int heigth)
{
	HRESULT hr = DD_OK;
	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, 
		NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw, (void**)&mDDrawObj));

	CHECK_HR(hr = CreateSurfaces(width, heigth));

	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));
	CHECK_HR(hr = mDDrawObj->SetDisplayMode(width, heigth, 16));

	mWidth = width;
	mHeight = heigth;

done:
	return hr;
}

HRESULT DDrawRender::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = DD_OK;
	mDDrawPrimarySurface->
	return hr;
}

HRESULT DDrawRender::DeinitDDrawInterface()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(mDDrawObj);
	SAFE_RELEASE(mDDrawPrimarySurface);
	SAFE_RELEASE(mDDrawSecondarySurface);

	return hr;
}