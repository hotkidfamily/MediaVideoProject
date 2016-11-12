// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"

IVideoCaptureDelegate::~IVideoCaptureDelegate()
{
}

bool IVideoCaptureDelegate::InitContext()
{
	mDShowCapture = new DShowVideoCapture;
	mMFCapture = new MediaFoundationVideoCapture; 
	mDShowCapture->GetDShowInterfaces();
	return (!mDShowCapture && !mMFCapture);
}

bool IVideoCaptureDelegate::UnInitContext()
{
	if (mDShowCapture){
		delete mDShowCapture;
		mDShowCapture = NULL;
	}

	if (mMFCapture){
		delete mMFCapture;
		mMFCapture = NULL;
	}

	return (!mDShowCapture && !mMFCapture);
}

void IVideoCaptureDelegate::RegisterCallback(VideoCaptureCallback *cb)
{
	mDShowCapture->RegisterCallback(cb);
}

HRESULT IVideoCaptureDelegate::GetDeviceList(VECT &camNames)
{
	HRESULT hr = S_OK;
	hr = mDShowCapture->EnumCaptureDevices();
	hr = mDShowCapture->GetDevicesName(camNames);
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

HRESULT IVideoCaptureDelegate::UnRegisterCallback()
{
	mDShowCapture->UnregisterCallback();
	return S_OK;
}