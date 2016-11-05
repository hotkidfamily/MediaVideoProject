// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"

IVideoCaptureDelegate::~IVideoCaptureDelegate()
{

}

void IVideoCaptureDelegate::RegisterVideoCaptureCallback(VideoCaptureCallback *cb)
{
	mCaptureCallback = cb;
}

HRESULT IVideoCaptureDelegate::GetDeviceList(std::vector<char*> &)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IVideoCaptureDelegate::StartCaptureByIndexWithResolutionAndFramerate(int, int, int, int)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IVideoCaptureDelegate::StopCapture()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IVideoCaptureDelegate::ShowPropertyWindow(HWND parentWindowHandle)
{
	HRESULT hr = S_OK;
	return hr;
}