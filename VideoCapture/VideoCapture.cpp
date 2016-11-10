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
	hr = mDShowCapture->EnumCaptureDevices();
	hr = mDShowCapture->GetDevices(camNames);
	return hr;
}

HRESULT IVideoCaptureDelegate::StartCaptureWithParam(OPEN_DEVICE_PARAM param)
{
	HRESULT hr = S_OK;
	hr = mDShowCapture->Start(param);
	return hr;
}

HRESULT IVideoCaptureDelegate::StopCapture()
{
	HRESULT hr = S_OK;
	mDShowCapture->Stop();
	return hr;
}

HRESULT IVideoCaptureDelegate::ShowPropertyWindow(HWND parentWindowHandle)
{
	HRESULT hr = S_OK;
	mDShowCapture->ShowCapturePropertyWindow();
	return hr;
}

HRESULT IVideoCaptureDelegate::Repaint(HDC hdc)
{
	HRESULT hr = S_OK;
	mDShowCapture->Repaint(hdc);
	return hr;
}

HRESULT IVideoCaptureDelegate::UpdateVideoWindow(HWND hWnd, const LPRECT pRC)
{
	HRESULT hr = S_OK;
	mDShowCapture->UpdateVideoWindow(hWnd, pRC);
	return hr;
}