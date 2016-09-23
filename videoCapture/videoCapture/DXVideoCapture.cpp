// VideoCapture.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "VideoCapture.h"
#include "DXVideoCapture.h"


DXVideoCapture::DXVideoCapture()
{
}

HRESULT DXVideoCapture::initEnv()
{
	HRESULT hr = E_FAIL;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGraphBuilder));
	if (FAILED(hr)){
		goto done;
	}

	hr = pGraphBuilder->QueryInterface(IID_PPV_ARGS(&pMediaControl));
	if (FAILED(hr)){
		goto done;
	}

	hr = pGraphBuilder->QueryInterface(IID_PPV_ARGS(&pMediaEvent));
	if (FAILED(hr)){
		goto done;
	}

done:
	return hr;
}

HRESULT DXVideoCapture::createSampleGrabberFilter()
{ 
	// Create the Sample Grabber filter.
	HRESULT hr = E_FAIL;

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pSampleGrabber));
	if (FAILED(hr)){
		goto done;
	}

	hr = pGraphBuilder->AddFilter(pSampleGrabber, L"Sample Grabber");
	if (FAILED(hr)){
		goto done;
	}

	hr = pSampleGrabber->QueryInterface(IID_PPV_ARGS(&pGrabber));
	if (FAILED(hr)){
		goto done;
	}

done:
	return hr;
}

HRESULT DXVideoCapture::setMediaType()
{
	HRESULT hr = E_FAIL;
	AM_MEDIA_TYPE mt;
	
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;

	hr = pGrabber->SetMediaType(&mt);
	if (FAILED(hr))
	{
		goto done;
	}

done:
	return hr;
}

HRESULT DXVideoCapture::buildFilterGraph(std::wstring videoFile)
{
	HRESULT hr = E_FAIL;

	hr = pGraphBuilder->AddSourceFilter(videoFile.c_str(), L"Source", &pSourceF);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSourceF->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		goto done;
	}

	while (S_OK == pEnum->Next(1, &pPin, NULL))
	{
		hr = ConnectFilters(pGraphBuilder, pPin, pSampleGrabber);
		SafeRelease(&pPin);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		goto done;
	}

done:
	return hr;
}

HRESULT DXVideoCapture::setSampleCallback(SampleBufferCallback &callback)
{
	if (!sample_cb)
		sample_cb = new ISampleGrabberCBImpl(callback);

	return S_OK;
}

HRESULT DXVideoCapture::runWithCallbackMode()
{
	HRESULT hr;

	hr = pGrabber->SetOneShot(FALSE);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGrabber->SetCallback(sample_cb, SAMPLE_CALLBACK_MODE);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pMediaControl->Run();
	if (FAILED(hr))
	{
		goto done;
	}

	long evCode;
	hr = pMediaEvent->WaitForCompletion(INFINITE, &evCode);

done:
	return hr;
}

HRESULT DXVideoCapture::runWithBufferMode()
{
	HRESULT hr;

	hr = pGrabber->SetOneShot(TRUE);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGrabber->SetBufferSamples(TRUE);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pMediaControl->Run();
	if (FAILED(hr))
	{
		goto done;
	}

	long evCode;
	hr = pMediaEvent->WaitForCompletion(INFINITE, &evCode);

done:
	return hr;
}

HRESULT DXVideoCapture::grabSample(std::wstring bmpFileName)
{
	HRESULT hr;
	AM_MEDIA_TYPE mt;

	// Find the required buffer size.
	long cbBuffer;
	hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
	if (FAILED(hr))
	{
		goto done;
	}

	pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
	if (!pBuffer)
	{
		hr = E_OUTOFMEMORY;
		goto done;
	}

	hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr))
	{
		goto done;
	}

	// Examine the format block.
	if ((mt.formattype == FORMAT_VideoInfo) &&
		(mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
		(mt.pbFormat != NULL))
	{
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)mt.pbFormat;

		hr = WriteBitmap(bmpFileName.c_str() , &pVih->bmiHeader, mt.cbFormat - SIZE_PREHEADER, pBuffer, cbBuffer);
	}
	else
	{
		// Invalid format.
		hr = VFW_E_INVALIDMEDIATYPE;
	}

	FreeMediaType(mt);

done:
	return hr;
}

// Query whether a pin has a specified direction (input / output)
HRESULT DXVideoCapture::IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult)
{
	PIN_DIRECTION pinDir;
	HRESULT hr = pPin->QueryDirection(&pinDir);
	if (SUCCEEDED(hr))
	{
		*pResult = (pinDir == dir);
	}
	return hr;
}


// Query whether a pin is connected to another pin.
//
// Note: This function does not return a pointer to the connected pin.

HRESULT DXVideoCapture::IsPinConnected(IPin *pPin, BOOL *pResult)
{
	IPin *pTmp = NULL;
	HRESULT hr = pPin->ConnectedTo(&pTmp);
	if (SUCCEEDED(hr))
	{
		*pResult = TRUE;
	}
	else if (hr == VFW_E_NOT_CONNECTED)
	{
		// The pin is not connected. This is not an error for our purposes.
		*pResult = FALSE;
		hr = S_OK;
	}

	SafeRelease(&pTmp);
	return hr;
}

// Match a pin by pin direction and connection state.
HRESULT DXVideoCapture::MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult)
{
	assert(pResult != NULL);

	BOOL bMatch = FALSE;
	BOOL bIsConnected = FALSE;

	HRESULT hr = IsPinConnected(pPin, &bIsConnected);
	if (SUCCEEDED(hr))
	{
		if (bIsConnected == bShouldBeConnected)
		{
			hr = IsPinDirection(pPin, direction, &bMatch);
		}
	}

	if (SUCCEEDED(hr))
	{
		*pResult = bMatch;
	}
	return hr;
}

HRESULT DXVideoCapture::FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	IEnumPins *pEnum = NULL;
	IPin *pPin = NULL;
	BOOL bFound = FALSE;

	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		goto done;
	}

	while (S_OK == pEnum->Next(1, &pPin, NULL))
	{
		hr = MatchPin(pPin, PinDir, FALSE, &bFound);
		if (FAILED(hr))
		{
			goto done;
		}
		if (bFound)
		{
			*ppPin = pPin;
			(*ppPin)->AddRef();
			break;
		}
		SafeRelease(&pPin);
	}

	if (!bFound)
	{
		hr = VFW_E_NOT_FOUND;
	}

done:
	SafeRelease(&pPin);
	SafeRelease(&pEnum);
	return hr;
}


HRESULT DXVideoCapture::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
	IPin *pOut = NULL;

	// Find an output pin on the first filter.
	HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
	if (SUCCEEDED(hr))
	{
		hr = ConnectFilters(pGraph, pOut, pDest);
		pOut->Release();
	}
	return hr;
}

HRESULT DXVideoCapture::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn)
{
	IPin *pOut = NULL;

	// Find an output pin on the upstream filter.
	HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
	if (SUCCEEDED(hr))
	{
		// Try to connect them.
		hr = pGraph->Connect(pOut, pIn);
		pOut->Release();
	}
	return hr;
}

HRESULT DXVideoCapture::ConnectFilters(
	IGraphBuilder *pGraph, // Filter Graph Manager.
	IPin *pOut,            // Output pin on the upstream filter.
	IBaseFilter *pDest)    // Downstream filter.
{
	IPin *pIn = NULL;

	// Find an input pin on the downstream filter.
	HRESULT hr = FindUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
	if (SUCCEEDED(hr))
	{
		// Try to connect them.
		hr = pGraph->Connect(pOut, pIn);
		pIn->Release();
	}
	return hr;
}

HRESULT DXVideoCapture::connectToNullRenderFilter()
{
	HRESULT hr;

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNullF));
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraphBuilder->AddFilter(pNullF, L"Null Render Filter");
	if (FAILED(hr))
	{
		goto done;
	}

	hr = ConnectFilters(pGraphBuilder, pSampleGrabber, pNullF);
	if (FAILED(hr))
	{
		goto done;
	}

done:
	return hr;
}

HRESULT DXVideoCapture::connectToVideoWindow(IVideoWindow *videoWindow)
{
	HRESULT hr = E_FAIL;

	return hr;
}

HRESULT DXVideoCapture::NotifyOwnerMessage(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam)
{
	return videoWindow->NotifyOwnerMessage(hWnd, msg, wParam, lParam);
}

void DXVideoCapture::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

void DXVideoCapture::releaseResources()
{
	CoTaskMemFree(pBuffer);
	SafeRelease(&pPin);
	SafeRelease(&pEnum);
	SafeRelease(&pNullF);
	SafeRelease(&pSourceF);
	SafeRelease(&pGrabber);
	SafeRelease(&pSampleGrabber);
	SafeRelease(&pMediaControl);
	SafeRelease(&pMediaEvent);
	SafeRelease(&pGraphBuilder);
}