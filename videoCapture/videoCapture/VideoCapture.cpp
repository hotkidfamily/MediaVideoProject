
#include "stdafx.h"
#include "VideoCapture.h"
#include "DXVideoCapture.h"
#include "VideoWindow.h"

static DXVideoCapture *cap = NULL;
static DXVideoWindows *window = NULL;

HRESULT VIDEOCAPTURE_API InitVideoCapture()
{
	if (!cap)
		cap = new DXVideoCapture;

	return S_OK;
}

HRESULT VIDEOCAPTURE_API DeinitVideoCapture()
{
	cap->releaseResources();
	return S_OK;
}

HRESULT VIDEOCAPTURE_API GrabVideoBitmap(PCWSTR pszVideoFile, PCWSTR pszBitmapFile)
{
	cap->initEnv();
	cap->createSampleGrabberFilter();
	cap->setMediaType();
	cap->buildFilterGraph(pszVideoFile);
	cap->connectToNullRenderFilter();
	cap->runWithBufferMode();
	cap->grabSample(pszBitmapFile);
	cap->releaseResources();

	return S_OK;
}

HRESULT VIDEOCAPTURE_API SetSampleCallback(SampleBufferCallback &cb)
{
	return S_OK;
}

HRESULT VIDEOCAPTURE_API MoveVideoWindowPos(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam)
{
	return cap->NotifyOwnerMessage(hWnd, msg, wParam, lParam);
}


// Writes a bitmap file
//  pszFileName:  Output file name.
//  pBMI:         Bitmap format information (including pallete).
//  cbBMI:        Size of the BITMAPINFOHEADER, including palette, if present.
//  pData:        Pointer to the bitmap bits.
//  cbData        Size of the bitmap, in bytes.

HRESULT WriteBitmap(PCWSTR pszFileName, BITMAPINFOHEADER *pBMI, size_t cbBMI,
	BYTE *pData, size_t cbData)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, 0, NULL);
	if (hFile == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	BITMAPFILEHEADER bmf = {};

	bmf.bfType = 'MB';
	bmf.bfSize = cbBMI + cbData + sizeof(bmf);
	bmf.bfOffBits = sizeof(bmf) + cbBMI;

	DWORD cbWritten = 0;
	BOOL result = WriteFile(hFile, &bmf, sizeof(bmf), &cbWritten, NULL);
	if (result)
	{
		result = WriteFile(hFile, pBMI, cbBMI, &cbWritten, NULL);
	}
	if (result)
	{
		result = WriteFile(hFile, pData, cbData, &cbWritten, NULL);
	}

	HRESULT hr = result ? S_OK : HRESULT_FROM_WIN32(GetLastError());

	CloseHandle(hFile);

	return hr;
}
