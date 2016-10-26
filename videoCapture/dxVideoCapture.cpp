#include "stdafx.h"
#include "dxVideoCapture.h"

#include <Dvdmedia.h>

#define CAPTURE_FILTER_NAME (TEXT("CAPTURE"))
#define RENDER_FILTER_NAME (TEXT("RENDER"))
#define GRABBER_FILTER_NAME (TEXT("Sample Grabber"))

dxVideoCapture::dxVideoCapture(logger &log)
	: log(log)
	, mFilterGraphVideoWindow(NULL)
	, m_hWnd(NULL)
{
}


dxVideoCapture::~dxVideoCapture()
{
}


HRESULT dxVideoCapture::getDSInterfaces(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (!IsWindow(hWnd)){
		goto done;
	}

	m_hWnd = hWnd;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&mFilterGraph);
	if (hr != S_OK){
		goto done;
	}

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&mFilterGraphBuilder);
	if (hr != S_OK){
		goto done;
	}

	hr = mFilterGraph->QueryInterface(IID_IMediaControl, (void**)&mFilterGraphMediaControl);
	if (hr != S_OK){
		goto done;
	}

	hr = mFilterGraph->QueryInterface(IID_IMediaEventEx, (void**)&mFilterGraphMediaEvent);
	if (hr != S_OK){
		goto done;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&mNullRenderFilter);
	if (hr != S_OK){
		goto done;
	}

	hr = mFilterGraph->QueryInterface(IID_IVideoWindow, (LPVOID *)&mFilterGraphVideoWindow);
	if (FAILED(hr)){
		goto done;
	}

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&mSampleGrabber);
	if (hr != S_OK){
		goto done;
	}

	hr = mSampleGrabber->QueryInterface(IID_IBaseFilter, (void**)&mSampleGrabberFilter);
	if (hr != S_OK){
		goto done;
	}

done:
	return hr;
}

HRESULT dxVideoCapture::configSampleGrabber()
{
	HRESULT hr = S_OK;
	AM_MEDIA_TYPE   mt;

	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = mSampleGrabber->SetMediaType(&mt);
	if (!SUCCEEDED(hr)){
		goto done;
	}

	mSampleGrabber->SetOneShot(FALSE);
	mSampleGrabber->SetBufferSamples(FALSE);

	mSampleGrabber->SetCallback(this, 0);

done:
	return hr;
}

HRESULT dxVideoCapture::setCaptureFormat()
{
	HRESULT hr = 0;

	//mFilterGraphBuilder->QueryInterface(PIN_CATEGORY_CAPTURE, )
	return hr;
}

HRESULT dxVideoCapture::getCaptureFormat()
{
	HRESULT hr = S_OK;
	AM_MEDIA_TYPE Type;
	BITMAPINFOHEADER * pVh;
	REFERENCE_TIME* rt;

	hr = mSampleGrabber->GetConnectedMediaType(&Type);
	if (hr == S_OK){
		pVh = dxUtils::getBmpHeader(&Type);
		rt = dxUtils::getFrameInterval(&Type);
		log.log(1, TEXT("Capture video in %dx%d, %f fps\n"), pVh->biWidth, pVh->biHeight, dxUtils::calcFps(*rt));
		dxUtils::FreeMediaType(Type);
	}
	else if (VFW_E_NOT_CONNECTED == hr ){
		log.log(1, TEXT("Grabber have not connected.\n"));
	}
	else {
		log.log(1, TEXT("Can not get connected media type..\n"));
	}

	return hr;
}

HRESULT dxVideoCapture::selectSuitablePin(IBaseFilter *filter)
{
	HRESULT hr = S_OK;



	return hr;
}

HRESULT dxVideoCapture::buildGraph(IBaseFilter* captureFilter)
{
	HRESULT hr = S_OK;
	AM_MEDIA_TYPE   mt;

	hr = mFilterGraphMediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);

	hr = mFilterGraphBuilder->SetFiltergraph(mFilterGraph);
	if (hr != S_OK){
		goto done;
	}

	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = mSampleGrabber->SetMediaType(&mt);
	if (!SUCCEEDED(hr)){
		goto done;
	}

	mSampleGrabber->SetOneShot(FALSE);
	mSampleGrabber->SetBufferSamples(FALSE);
	mSampleGrabber->SetCallback(this, 0);

	hr = mFilterGraph->AddFilter(captureFilter, CAPTURE_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}

	hr = mFilterGraph->AddFilter(mSampleGrabberFilter, GRABBER_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}

	hr = mFilterGraph->AddFilter(mNullRenderFilter, RENDER_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}

	
	setCaptureFormat();

done:
	return hr;
}

STDMETHODIMP dxVideoCapture::SampleCB(double SampleTime, IMediaSample *pSample)
{
	statics.appendDataSize(1);
	return S_OK;
}

HRESULT dxVideoCapture::showFilterPropertyPages(IBaseFilter *filter)
{
	ISpecifyPropertyPages *pProp = NULL;
	HRESULT hr = S_OK;
	hr = filter->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pProp);
	if (SUCCEEDED(hr)){
		FILTER_INFO filterInfo;
		hr = filter->QueryFilterInfo(&filterInfo);
		if (SUCCEEDED(hr)){
			IUnknown *pFilterUnk;
			hr = filter->QueryInterface(IID_IUnknown, (void**)&pFilterUnk);
			if (SUCCEEDED(hr)){
				CAUUID caGuid;
				pProp->GetPages(&caGuid);
				OleCreatePropertyFrame(NULL, 0, 0,
					filterInfo.achName, 1, &pFilterUnk, caGuid.cElems, caGuid.pElems, 0, 0, NULL);
				pFilterUnk->Release();
				CoTaskMemFree(caGuid.pElems);
			}
			if (filterInfo.pGraph)
				filterInfo.pGraph->Release();
		}
		pProp->Release();
	}

	return hr;
}

bool dxVideoCapture::isRuning()
{
	OAFilterState state = State_Stopped;
	if (mFilterGraphMediaControl)
		mFilterGraphMediaControl->GetState(INFINITE, &state);

	return state != State_Stopped;
}

HRESULT dxVideoCapture::start()
{
	HRESULT hr = E_FAIL;
	IBaseFilter* captureFilter = NULL;
	REFERENCE_TIME *rt = NULL;
	BITMAPINFOHEADER *pvh = NULL;

	hr = mFilterGraph->FindFilterByName(CAPTURE_FILTER_NAME, &captureFilter);
	if (SUCCEEDED(hr)){
		hr = mFilterGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, captureFilter, NULL, NULL);

		hr = mFilterGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, captureFilter, mSampleGrabberFilter, mNullRenderFilter);
		if (FAILED(hr)){
			goto done;
		}

		captureFilter->Release();
	}

	getCaptureFormat();
	SetupVideoWindow();

	hr = mFilterGraphMediaControl->Run();

	statics.reset(5000);

done:
	return hr;
}

void dxVideoCapture::ResizeVideoWindow(void)
{
	// Resize the video preview window to match owner window size
	if (mFilterGraphVideoWindow)
	{
		RECT rc;

		// Make the preview video fill our window
		GetClientRect(m_hWnd, &rc);
		mFilterGraphVideoWindow->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
}

HRESULT dxVideoCapture::SetupVideoWindow(void)
{
	HRESULT hr;

	// Set the video window to be a child of the main window
	hr = mFilterGraphVideoWindow->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr)){
		goto done;
	}

	// Set video window style
	hr = mFilterGraphVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr)){
		goto done;
	}

	// Use helper function to position video window in client rect 
	// of main application window
	ResizeVideoWindow();

	// Make the video window visible, now that it is properly positioned
	hr = mFilterGraphVideoWindow->put_Visible(OATRUE);
	if (FAILED(hr)){
		goto done;
	}

done:		
	return hr;
}

HRESULT dxVideoCapture::removeFilters()
{
	HRESULT hr;
	IBaseFilter *filter = NULL;
	hr = mFilterGraph->FindFilterByName(CAPTURE_FILTER_NAME, &filter);
	if (SUCCEEDED(hr)){
		filter->Stop();
	}

	/* must stop before remove from filter graph */
	mFilterGraphMediaControl->StopWhenReady();
	if (mFilterGraphVideoWindow){
		mFilterGraphVideoWindow->put_Visible(OAFALSE);
		mFilterGraphVideoWindow->put_Owner(NULL);
	}

	IEnumFilters *filterEnum;
	hr = mFilterGraph->EnumFilters(&filterEnum);
	if (SUCCEEDED(hr)){
		while (filterEnum->Next(1, &filter, NULL) == S_OK){
			FILTER_INFO info;
			filter->QueryFilterInfo(&info);
			mFilterGraph->RemoveFilter(filter);
			filterEnum->Reset();
			filter->Release();
		}
		filterEnum->Release();
	}

	return S_OK;
}

HRESULT dxVideoCapture::stop()
{
	removeFilters();

	log.log(1, TEXT("capture %f fps\n"), statics.frequencyPerSecond());

	return S_OK;
}


HRESULT dxVideoCapture::HandleGraphEvent(void)
{
	LONG evCode;
	LONG_PTR evParam1, evParam2;
	HRESULT hr = S_OK;

	if (!mFilterGraphMediaEvent)
		return E_POINTER;

	while (SUCCEEDED(mFilterGraphMediaEvent->GetEvent(&evCode, &evParam1, &evParam2, 0)))
	{
		//
		// Free event parameters to prevent memory leaks associated with
		// event parameter data.  While this application is not interested
		// in the received events, applications should always process them.
		//
		hr = mFilterGraphMediaEvent->FreeEventParams(evCode, evParam1, evParam2);

		// Insert event processing code here, if desired
	}

	return hr;
}