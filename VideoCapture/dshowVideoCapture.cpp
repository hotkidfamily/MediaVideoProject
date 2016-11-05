#include "stdafx.h"

#include "logger.h"
#include "dshowVideoCapture.h"



DShowVideoCapture::DShowVideoCapture()
{
	CoInitialize(NULL);
}

DShowVideoCapture::~DShowVideoCapture()
{

}

HRESULT DShowVideoCapture::GetDShowInterfaces()
{
	HRESULT hr = S_OK;

	CHECK_HR(CoCreateInstance(CLSID_FilterGraph, NULL, 
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&mGraph));
	CHECK_HR(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, 
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&mGraphBuiler));
	CHECK_HR(CoCreateInstance(CLSID_SampleGrabber, NULL, 
		CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&mGrabber));
	CHECK_HR(mGraph->QueryInterface(IID_IMediaControl, (void**)&mMediaControl));
	CHECK_HR(mGraph->QueryInterface(IID_IMediaEventEx, (void**)&mMediaEventEx));
	CHECK_HR(mGraphBuiler->SetFiltergraph(mGraph));
	CHECK_HR(mGraph->QueryInterface(IID_IVideoWindow, (void**)&mVideoWindow));

done:
	if(!SUCCEEDED(hr)){
		internel_log(Error, "Get dshow interface error: %d", hr);
	}

	return hr;
}

HRESULT DShowVideoCapture::ReleaseDShowInterfaces()
{
	HRESULT hr = S_OK;

	//RemoveFilter(mGraph)

	SAFE_RELEASE(mMediaEventEx);
	SAFE_RELEASE(mVideoWindow);
	SAFE_RELEASE(mGraph);

	return hr;
}

HRESULT DShowVideoCapture::BuildGraph()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT DShowVideoCapture::EnumCaptureDevices()
{
	HRESULT hr = S_OK;



	return hr;
}