// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"

IVideoCaptureDelegate::~IVideoCaptureDelegate()
{

}

void IVideoCaptureDelegate::RegisterCallback(VideoCaptureCallback *cb)
{
	mCaptureCallback = cb;
}

HRESULT IVideoCaptureDelegate::GetDeviceList(std::vector<const TCHAR*> &camNames)
{
	HRESULT hr = S_OK;
	hr = mDShowCapture->GetDevices(camNames);
	return hr;
}

HRESULT IVideoCaptureDelegate::StartCaptureWithParam(OPEN_DEVICE_PARAM param)
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