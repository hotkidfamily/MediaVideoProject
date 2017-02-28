// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"

IVideoCaptureDelegate::IVideoCaptureDelegate()
	:mDevice(CAP_DEV_NONE)
{
}

IVideoCaptureDelegate::~IVideoCaptureDelegate()
{
}

bool IVideoCaptureDelegate::InitContext()
{
	mDShowCapture = new DShowVideoCapture(mBaseClock);
	mDShowCapture->GetDShowInterfaces();
	mFilesCapture = new FilesVideoCapture(mBaseClock);
	
	return (!mDShowCapture && !mFilesCapture);
}

bool IVideoCaptureDelegate::UnInitContext()
{
	if (mDShowCapture){
		delete mDShowCapture;
		mDShowCapture = nullptr;
	}

	if (mFilesCapture){
		delete mFilesCapture;
		mFilesCapture = nullptr;
	}

	return (!mDShowCapture && !mFilesCapture);
}

void IVideoCaptureDelegate::RegisterCallback(VideoCaptureCallback *cb)
{
	if (mDevice == CAP_DEV_DSHOW){
		mDShowCapture->RegisterCallback(cb);
	} else{
		mFilesCapture->RegisterCallback(cb);
	}	
}

HRESULT IVideoCaptureDelegate::GetDeviceList(VECT &camNames)
{
	HRESULT hr = S_OK;
	hr = mDShowCapture->EnumCaptureDevices();
	hr = mDShowCapture->GetDevicesName(camNames);
	return hr;
}

HRESULT IVideoCaptureDelegate::StartCaptureWithParam(CAPTURECONFIG &param)
{
	HRESULT hr = S_OK;

	if (!param.filePath.empty()){
		mDevice = CAP_DEV_FILE;
		hr = mFilesCapture->StartCaptureWithParam(param);
	} else{
		mDevice = CAP_DEV_DSHOW;
		hr = mDShowCapture->Start(param);
	}
	return hr;
}

HRESULT IVideoCaptureDelegate::StopCapture()
{
	HRESULT hr = S_OK;

	if (mDevice == CAP_DEV_DSHOW){
		hr = mDShowCapture->Stop();
	} else{
		hr = mFilesCapture->StopCapture();
	}

	return hr;
}

BOOL IVideoCaptureDelegate::GetFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;

	if (mDevice == CAP_DEV_DSHOW){
		bRet = mDShowCapture->GetFrame(pSample);
	} else{
		bRet = mFilesCapture->GetFrame(pSample);
	}

	return bRet;
}

BOOL IVideoCaptureDelegate::ReleaseFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;

	if (mDevice == CAP_DEV_DSHOW){
		bRet = mDShowCapture->ReleaseFrame(pSample);
	} else{
		bRet = mFilesCapture->ReleaseFrame(pSample);
	}

	return bRet;
}

HRESULT IVideoCaptureDelegate::ShowPropertyWindow(HWND parentWindowHandle)
{
	HRESULT hr = S_OK;

	if (mDevice == CAP_DEV_DSHOW){
		hr = mDShowCapture->ShowCapturePropertyWindow();
	} else{
		hr = mFilesCapture->ShowPropertyWindow(NULL);
	}

	return hr;
}

HRESULT IVideoCaptureDelegate::UnRegisterCallback()
{
	HRESULT hr = S_OK;

	if (mDevice == CAP_DEV_DSHOW){
		hr = mDShowCapture->UnregisterCallback();
	} else{
		hr = mFilesCapture->UnRegisterCallback();
	}

	return hr;
}
