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
		mDShowCapture = nullptr;
	}

	if (mMFCapture){
		delete mMFCapture;
		mMFCapture = nullptr;
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

HRESULT IVideoCaptureDelegate::StartCaptureWithParam(OPEN_DEVICE_PARAM &param)
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

BOOL IVideoCaptureDelegate::GetFrame(CSampleBuffer *&pSample)
{
	return mDShowCapture->GetFrame(pSample);
}

BOOL IVideoCaptureDelegate::ReleaseFrame(CSampleBuffer *&pSample)
{
	return mDShowCapture->ReleaseFrame(pSample);
}

HRESULT IVideoCaptureDelegate::ShowPropertyWindow(HWND parentWindowHandle)
{
	HRESULT hr = S_OK;
	mDShowCapture->ShowCapturePropertyWindow();
	return hr;
}

HRESULT IVideoCaptureDelegate::UnRegisterCallback()
{
	mDShowCapture->UnregisterCallback();
	return S_OK;
}