#include "stdafx.h"
#include "dxVideoCapture.h"

#include <Dvdmedia.h>

#define CAPTURE_FILTER_NAME (TEXT("CAPTURE"))
#define RENDER_FILTER_NAME (TEXT("RENDER"))
#define GRABBER_FILTER_NAME (TEXT("Sample Grabber"))

dxVideoCapture::dxVideoCapture(logger &log)
	: log(log)
	, m_pVM(NULL)
	, m_hWnd(NULL)
{
}


dxVideoCapture::~dxVideoCapture()
{
}


HRESULT dxVideoCapture::initGraph(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (!IsWindow(hWnd)){
		goto done;
	}

	m_hWnd = hWnd;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_filterGraph);
	if (hr != S_OK){
		goto done;
	}

	hr = m_filterGraph->QueryInterface(IID_IMediaControl, (void**)&m_mediaControl);
	if (hr != S_OK){
		goto done;
	}

	hr = m_filterGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_MediaEvent);
	if (hr != S_OK){
		goto done;
	}

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_captureGraphBuilder);
	if (hr != S_OK){
		goto done;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_nullRenderFilter);
	if (hr != S_OK){
		goto done;
	}

	hr = m_filterGraph->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVM);
	if (FAILED(hr)){
		goto done;
	}

	hr = m_MediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);

	hr = m_captureGraphBuilder->SetFiltergraph(m_filterGraph);
	if (hr != S_OK){
		goto done;
	}

	hr = openSampleGrabber();
	if (FAILED(hr)){
		goto done;
	}

done:
	return hr;
}

HRESULT dxVideoCapture::openSampleGrabber()
{
	HRESULT hr = S_OK;
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&m_sampleGrabber);
	if (hr != S_OK){
		goto done;
	}

	hr = m_sampleGrabber->QueryInterface(IID_IBaseFilter, (void**)&m_sampleGrabberFilter);
	if (hr != S_OK){
		goto done;
	}

	/* set media type */
	AM_MEDIA_TYPE   mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	m_sampleGrabber->SetMediaType(&mt);

	m_sampleGrabber->SetOneShot(FALSE);
	m_sampleGrabber->SetBufferSamples(FALSE);

	m_sampleGrabber->SetCallback(this, 0);

done:
	return hr;
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

HRESULT dxVideoCapture::enumPins(IBaseFilter *captureFilter)
{
	HRESULT hr = E_FAIL;
	ULONG ulCnt = 0;

	IEnumPins *pEnum = NULL;
	IPin *pPins = NULL;

	int pinCnt = 0;
	
	hr = captureFilter->EnumPins(&pEnum);
	if (FAILED(hr)){
		goto done;
	}

	while (pEnum->Next(1, &pPins, &ulCnt) == S_OK){
		PIN_INFO pinInfo = { 0 };

		if (pPins->QueryPinInfo(&pinInfo) == S_OK){
			log.log(1, TEXT("pin %d,  %s\n"), pinCnt++, pinInfo.dir == PINDIR_INPUT ? TEXT("INPUT") : TEXT("OUTPUT"));
				
			IAMStreamConfig *streamCfg = NULL;
			hr = pPins->QueryInterface(IID_IAMStreamConfig, (void**)&streamCfg);
			if (SUCCEEDED(hr)){
				int count = 0;
				int size = 0;
				VIDEO_STREAM_CONFIG_CAPS *pCaps = NULL;

				hr = streamCfg->GetNumberOfCapabilities(&count, &size);
				if (hr != S_OK){
					continue;
				}
				if (size != sizeof(VIDEO_STREAM_CONFIG_CAPS)){
					continue;
				}

				pCaps = new VIDEO_STREAM_CONFIG_CAPS;
				int idx = 0;
				for (int i = 0; i < count; i++){
					AM_MEDIA_TYPE *pmt = NULL;
					if (streamCfg->GetStreamCaps(i, &pmt, (BYTE*)pCaps) == S_OK){
						BITMAPINFOHEADER * pvh = dxUtils::getBmpHeader(pmt);
						log.log(1, TEXT("\t\t %3d %4dx%4d fps: %f ~ %f (%s,%s,%s)\n"), idx++, pvh->biWidth, abs(pvh->biHeight),
							dxUtils::calcFps(pCaps->MaxFrameInterval), dxUtils::calcFps(pCaps->MinFrameInterval), dxUtils::getInfo(pmt->majortype, UNUSED_FOURCC_CODE),
							dxUtils::getInfo(pmt->subtype, pvh->biCompression), dxUtils::getInfo(pmt->formattype, UNUSED_FOURCC_CODE));
						dxUtils::FreeMediaType(*pmt);
					}					
				}
				delete pCaps;
				streamCfg->Release(); 
			}else{
				log.log(1, TEXT("\n"));
			}
		}
		pPins->Release();
	}
	pEnum->Release();

done:
	return hr;
}

HRESULT dxVideoCapture::buildGraph(IBaseFilter* captureFilter)
{
	HRESULT hr = S_OK;

	enumPins(captureFilter);

	hr = m_filterGraph->AddFilter(m_nullRenderFilter, RENDER_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}
	hr = m_filterGraph->AddFilter(m_sampleGrabberFilter, GRABBER_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}
	hr = m_filterGraph->AddFilter(captureFilter, CAPTURE_FILTER_NAME);
	if (S_OK != hr){
		goto done;
	}

done:
	return hr;
}

STDMETHODIMP dxVideoCapture::SampleCB(double SampleTime, IMediaSample *pSample)
{
	statics.appendDataSize(1);
	return S_OK;
}

bool dxVideoCapture::isRuning()
{
	OAFilterState state = State_Stopped;
	if (m_mediaControl)
		m_mediaControl->GetState(INFINITE, &state);

	return state != State_Stopped;
}

HRESULT dxVideoCapture::start()
{
	HRESULT hr = E_FAIL;
	AM_MEDIA_TYPE   *pmt = NULL;
	AM_MEDIA_TYPE   mt;
	IBaseFilter* captureFilter = NULL;
	REFERENCE_TIME *rt = NULL;
	BITMAPINFOHEADER *pvh = NULL;

	hr = m_filterGraph->FindFilterByName(CAPTURE_FILTER_NAME, &captureFilter);
	if (SUCCEEDED(hr)){
		hr = m_captureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, captureFilter, NULL, NULL);

		hr = m_captureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, captureFilter, m_sampleGrabberFilter, m_nullRenderFilter);
		if (FAILED(hr)){
			goto done;
		}
		IAMStreamConfig *streamCfg = NULL;
		hr = m_captureGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, captureFilter, IID_IAMStreamConfig, (void**)&streamCfg);
		if (hr != S_OK){
			goto done;
		}
		streamCfg->GetFormat(&pmt);
		pvh = dxUtils::getBmpHeader(pmt);
		rt = dxUtils::getFrameInterval(pmt);

		if (pvh){
			pvh->biWidth = 1280;
			pvh->biHeight = 720;
			*rt = (REFERENCE_TIME)(10000000 / 15);
		}
		hr = streamCfg->SetFormat(pmt);
		if (FAILED(hr)){
			log.log(1, TEXT("can not config camera .\n"));
		}

		dxUtils::FreeMediaType(*pmt);
	}

	SetupVideoWindow();

	hr = m_sampleGrabber->GetConnectedMediaType(&mt);
	if (hr == S_OK){
		BITMAPINFOHEADER *pBmp = NULL;
		REFERENCE_TIME *AvgTimePerFrame = NULL;

		pBmp = dxUtils::getBmpHeader(pmt);
		AvgTimePerFrame = dxUtils::getFrameInterval(pmt);
		if (pBmp && AvgTimePerFrame)
			log.log(1, TEXT("open camera with %dx%d, %f fps.\n"), pBmp->biWidth, abs(pBmp->biHeight), dxUtils::calcFps(*AvgTimePerFrame));

		dxUtils::FreeMediaType(mt);
	}else if (hr == VFW_E_NOT_CONNECTED){
		log.log(1, TEXT("filter have not connected.\n"));
	}

	hr = m_mediaControl->Run();

	statics.reset(5000);

done:
	return hr;
}

void dxVideoCapture::ResizeVideoWindow(void)
{
	// Resize the video preview window to match owner window size
	if (m_pVM)
	{
		RECT rc;

		// Make the preview video fill our window
		GetClientRect(m_hWnd, &rc);
		m_pVM->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
}

HRESULT dxVideoCapture::SetupVideoWindow(void)
{
	HRESULT hr;

	// Set the video window to be a child of the main window
	hr = m_pVM->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr)){
		goto done;
	}

	// Set video window style
	hr = m_pVM->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr)){
		goto done;
	}

	// Use helper function to position video window in client rect 
	// of main application window
	ResizeVideoWindow();

	// Make the video window visible, now that it is properly positioned
	hr = m_pVM->put_Visible(OATRUE);
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
	hr = m_filterGraph->FindFilterByName(CAPTURE_FILTER_NAME, &filter);
	if (SUCCEEDED(hr)){
		filter->Stop();
	}

	/* must stop before remove from filter graph */
	m_mediaControl->StopWhenReady();
	if (m_pVM){
		m_pVM->put_Visible(OAFALSE);
		m_pVM->put_Owner(NULL);
	}

	IEnumFilters *filterEnum;
	hr = m_filterGraph->EnumFilters(&filterEnum);
	if (SUCCEEDED(hr)){
		while (filterEnum->Next(1, &filter, NULL) == S_OK){
			FILTER_INFO info;
			filter->QueryFilterInfo(&info);
			m_filterGraph->RemoveFilter(filter);
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

	if (!m_MediaEvent)
		return E_POINTER;

	while (SUCCEEDED(m_MediaEvent->GetEvent(&evCode, &evParam1, &evParam2, 0)))
	{
		//
		// Free event parameters to prevent memory leaks associated with
		// event parameter data.  While this application is not interested
		// in the received events, applications should always process them.
		//
		hr = m_MediaEvent->FreeEventParams(evCode, evParam1, evParam2);

		// Insert event processing code here, if desired
	}

	return hr;
}