#include "stdafx.h"

#include "logger.h"
#include "dshowVideoCapture.h"
#include "dshowutil.h"

#define CAPTURE_FILTER_NAME_STR (TEXT("Capture Filter"))
#define RENDER_FILTER_NAME_STR (TEXT("NULL Render Filter"))
#define GRABBER_FILTER_NAME_STR (TEXT("Grabber Filter"))

using namespace ATL;

DShowVideoCapture::DShowVideoCapture()
	: mGraph(NULL)
	, mGraphBuiler(NULL)
	, mMediaControl(NULL)
	, mMediaEventEx(NULL)
	, mRender(NULL)
	, mDropFrameStatus(NULL)
	, mCaptureFilter(NULL)
	, mGrabberFiler(NULL)
	, mGrabber(NULL)
	, mcb(NULL)
{
}

DShowVideoCapture::~DShowVideoCapture()
{
	internel_log(Info, "fps %f", mFpsStats.frequencyPerSecond());
}

void DShowVideoCapture::ShowDShowError(HRESULT hr)
{
	if (FAILED(hr)){
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		internel_log(Info, "dshow Error %s", errMsg);
	}
}

HRESULT DShowVideoCapture::RegisterCallback(VideoCaptureCallback *cb)
{
	mcb = cb;
	return S_OK;
}

HRESULT DShowVideoCapture::UnregisterCallback()
{
	mcb = NULL;
	return S_OK;
}

HRESULT DShowVideoCapture::GetDShowInterfaces()
{
	HRESULT hr = S_OK;

	CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL, 
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&mGraph));
	CHECK_HR(hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&mGraphBuiler));
	CHECK_HR(hr = mGraphBuiler->SetFiltergraph(mGraph));

	CHECK_HR(hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
		CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&mGrabber));
	CHECK_HR(hr = mGrabber->QueryInterface(IID_IBaseFilter, (void**)&mGrabberFiler));

	CHECK_HR(hr = mGraph->QueryInterface(IID_IMediaControl, (void**)&mMediaControl));
	CHECK_HR(hr = mGraph->QueryInterface(IID_IMediaEventEx, (void**)&mMediaEventEx));

done:
	ShowDShowError(hr);

	return hr;
}

bool DShowVideoCapture::Runing()
{
	HRESULT hr = S_OK;
	bool bRet = true;
	OAFilterState status = State_Stopped;

	ASSERT(mMediaControl);
	
	hr = mMediaControl->GetState(INFINITE, &status);
	if (status == State_Stopped || FAILED(hr)){
		bRet = false;
	}
	ShowDShowError(hr);
	return bRet;
}

HRESULT DShowVideoCapture::Start(OPEN_DEVICE_PARAM params)
{
	HRESULT hr = S_OK;
	ASSERT(mMediaControl);

	mWorkParams = params;

	mRender = new VMR7();
	ASSERT(mRender);

	BuildGraph();

	while (hr = mMediaControl->Run() != S_OK){
		Sleep(100);
	}

	if (mDropFrameStatus){
		mDropFrameStatus->GetNumDropped(&mDropFrames);
		mDropFrameStatus->GetNumNotDropped(&mCapFrames);
	}

	ShowDShowError(hr);

	return hr;
}

HRESULT DShowVideoCapture::Stop()
{
	ASSERT(mMediaControl);

	SAFE_DELETE(mRender);

	if (mDropFrameStatus){
		long capFrameCount = 0;
		long dropFrameCount = 0;
		mDropFrameStatus->GetNumDropped(&dropFrameCount);
		mDropFrameStatus->GetNumNotDropped(&capFrameCount);
		internel_log(Info, "capture statistics: capture %ld, drop %ld",
			dropFrameCount - mDropFrames, capFrameCount - mCapFrames);
	}

	return mMediaControl->Stop();
}

HRESULT DShowVideoCapture::SampleCB(double SampleTime, IMediaSample *pSample)
{
	FRAME_DESC desc;
	HRESULT  hr = S_OK;

	ASSERT(mcb != NULL);

	CHECK_HR(hr = pSample->GetPointer(&desc.dataPtr));
	desc.dataSize = pSample->GetActualDataLength();
	hr = pSample->GetMediaTime(&desc.frameStartIdx, &desc.frameEndIdx);

	hr = pSample->GetTime(&desc.ptsStart, &desc.ptsEnd);
	if (FAILED(hr)){
		desc.ptsStart = timeGetTime();
		desc.ptsEnd = desc.ptsStart + RefTimeToMsec(mWorkParams.avgFrameIntervalInNs);
	}

	mcb->OnFrame(desc);

done:
	mFpsStats.appendDataSize(1);

	return hr;
}

HRESULT DShowVideoCapture::Repaint(HDC hdc)
{
	ASSERT(mRender != NULL);

	return mRender->Repaint(mWorkParams.parentWindow, hdc);
}

HRESULT DShowVideoCapture::UpdateVideoWindow(HWND hWnd, const LPRECT prc)
{
	ASSERT(mRender != NULL);

	return mRender->UpdateVideoWindow(hWnd, prc);
}

HRESULT DShowVideoCapture::ShowCapturePropertyWindow()
{
	HRESULT hr = S_OK;
	ASSERT(mGraph);
	ASSERT(mCaptureFilter);

	hr = ShowFilterPropertyPage(mCaptureFilter, mWorkParams.parentWindow);

	return hr;
}

//---------------------------------------------------------
// do not care return value
//
//---------------------------------------------------------
HRESULT DShowVideoCapture::RemoveFiltersFromGraph()
{
	HRESULT hr = S_OK;
	CComPtr<IEnumFilters> pFilterEnum = NULL;
	CComPtr<IBaseFilter> pFilter = NULL;

	ASSERT(mGraph);

	CHECK_HR(mGraph->EnumFilters(&pFilterEnum));
	while (pFilterEnum->Next(1, &pFilter, NULL) == S_OK){
		mGraph->RemoveFilter(pFilter);
		pFilter.Release();
		pFilterEnum->Reset();
	}
	pFilterEnum.Release();

done:
	
	return hr;
}

HRESULT DShowVideoCapture::ReleaseDShowInterfaces()
{
	HRESULT hr = S_OK;

	if (Runing())
		Stop();

	RemoveFiltersFromGraph();

	SAFE_RELEASE(mDropFrameStatus);
	SAFE_RELEASE(mMediaEventEx);
	SAFE_RELEASE(mMediaControl);
	SAFE_RELEASE(mGrabberFiler);
	SAFE_RELEASE(mGraphBuiler);
	SAFE_RELEASE(mGraph);

	if (mRender){
		delete mRender;
		mRender = NULL;
	}
	
	return hr;
}

HRESULT DShowVideoCapture::BuildGraph()
{
	HRESULT hr = E_FAIL;
	CComPtr<IBaseFilter> pNullRenderFilter = NULL;
	CComPtr<IPin> pCaptureOutPin = NULL;
	CComPtr<IPin> pRenderInPin = NULL;
	CComPtr<IPin> pGrabberInPin = NULL;
	CComPtr<IPin> pGrabberOutPin = NULL;

	CMediaType mediaType;
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_YUY2;

	CHECK_HR(CoCreateInstance(CLSID_NullRenderer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderFilter));
	CHECK_HR(hr = FindFilterByIndex(mWorkParams.index, mCaptureFilter));
	ASSERT(mCaptureFilter);

	CHECK_HR(hr = mGraph->AddFilter(mCaptureFilter, CAPTURE_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(mGrabberFiler, GRABBER_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(pNullRenderFilter, RENDER_FILTER_NAME_STR));

	CHECK_HR(hr = FindSultablePin(pCaptureOutPin));
	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_OUTPUT, &pGrabberOutPin));
	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_INPUT, &pGrabberInPin));
	CHECK_HR(hr = FindUnconnectedPin(pNullRenderFilter, PINDIR_INPUT, &pRenderInPin));
	CHECK_HR(hr = mGraph->Connect(pCaptureOutPin, pGrabberInPin));
	CHECK_HR(hr = mGraph->Connect(pGrabberOutPin, pRenderInPin));

	CHECK_HR(hr = mGrabber->SetMediaType(&mediaType));
	CHECK_HR(hr = mGrabber->SetCallback(this, 0));
	CHECK_HR(hr = mGrabber->SetOneShot(FALSE));
	
	CHECK_HR(hr = mRender->AddToGraph(mGraph, mWorkParams.parentWindow));
	CHECK_HR(hr = mRender->FinalizeGraph(mGraph));

	mGraphBuiler->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, mCaptureFilter, NULL, NULL);

	CHECK_HR(hr = mGraphBuiler->RenderStream(&PIN_CATEGORY_CAPTURE, 
		&MEDIATYPE_Video, mCaptureFilter, mGrabberFiler, pNullRenderFilter));

done:
	ShowDShowError(hr);
	return hr;
}

HRESULT DShowVideoCapture::GetDevicesName(VECT &cameNames)
{
	HRESULT hr = E_FAIL;

	if (mCameraList.size() != 0){
		CAMERALIST::iterator it = mCameraList.begin();
		for (; it != mCameraList.end(); it++){
			cameNames.push_back(it->name.c_str());
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT DShowVideoCapture::FindVideoConfigByStreamConfig(CComPtr<IAMStreamConfig> &pConfig)
{
	HRESULT hr = E_FAIL;
	int cfgCnt = 0;
	int cfgSize = 0;

	ASSERT(pConfig);

	CHECK_HR(hr = pConfig->GetNumberOfCapabilities(&cfgCnt, &cfgSize));
	for (int i = 0; i < cfgCnt; i++){
		CMediaType *mediaType = NULL;
		VIDEO_STREAM_CONFIG_CAPS caps;
		if (hr = pConfig->GetStreamCaps(i, (AM_MEDIA_TYPE**)&mediaType, (BYTE*)&caps) == S_OK){
			if (mediaType->isVideoInfoHeader()){
				if (mWorkParams.avgFrameIntervalInNs >= caps.MinFrameInterval
					&& mWorkParams.avgFrameIntervalInNs <= caps.MaxFrameInterval){

					BITMAPINFOHEADER *bmp = mediaType->BitmapHeader();
					if (mWorkParams.width == bmp->biWidth && mWorkParams.height == bmp->biHeight){

						mWorkMediaType.Set(*mediaType);

						hr = S_OK;
						break;
					}
				}
			}
		}
	}

done:
	ShowDShowError(hr);
	return hr;
}

HRESULT DShowVideoCapture::FindSultablePin(CComPtr<IPin> &pOutPin)
{
	HRESULT hr = S_OK;
	CComPtr<IEnumPins> pPinEnums = NULL;
	CComPtr<IPin> pPin = NULL;
	CComPtr<IPin> pGrabberInPin = NULL;
	CComPtr<IAMStreamConfig> pConfig = NULL;
	
	CHECK_HR(hr = mCaptureFilter->EnumPins(&pPinEnums));
	pPinEnums->Reset();
	while ((hr = pPinEnums->Next(1, &pPin, NULL)) == S_OK){
		PIN_DIRECTION dir;
		pPin->QueryDirection(&dir);
		if (dir != PINDIR_OUTPUT){
			pPin.Release();
			continue;
		}
		hr = pPin->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig);
		if ((hr = FindVideoConfigByStreamConfig(pConfig)) == S_OK){
			break;
		}
		pPin.Release();
		pConfig.Release();
	}

	pOutPin = pPin;
	
	mWorkMediaType.SetAvgReferenceTime(mWorkParams.avgFrameIntervalInNs);
	CHECK_HR(hr = pConfig->SetFormat(&mWorkMediaType));
	mWorkMediaType.GUIDtoStr(mWorkMediaType.subtype);

done:
	ShowDShowError(hr);

	pPinEnums.Release();
	pPin.Release();
	pConfig.Release();
	pGrabberInPin.Release();
	return hr;
}

HRESULT DShowVideoCapture::FindFilterByIndex(int index, IBaseFilter * &filter)
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> pDevEnum = NULL;
	CComPtr<IEnumMoniker> pDevEnumMoniker = NULL;
	CComPtr<IMoniker> pM = NULL;
	CComPtr<IPropertyBag> pProperty = NULL;

	CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum));
	CHECK_HR(hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pDevEnumMoniker, 0));

	pDevEnumMoniker->Reset();
	while (hr = pDevEnumMoniker->Next(1, &pM, NULL) == S_OK){
		VARIANT name;
		VARIANT path;
		VariantInit(&name);
		VariantInit(&path);
		if (pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProperty) == S_OK){
			pProperty->Read(TEXT("FriendlyName"), &name, 0);
			pProperty->Read(TEXT("DevicePath"), &path, 0);

			CAMERADESC dev(name, path);

			if (mCameraList[index] == dev){
				hr = pM->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&filter);
				filter->QueryInterface(IID_IAMDroppedFrames, (void**)&mDropFrameStatus);
				break;
			}
		}
		VariantClear(&name);
		VariantClear(&path);
		pM.Release();
	}

done:
	ShowDShowError(hr);

	pDevEnum.Release();
	pDevEnumMoniker.Release();
	pM.Release();
	pProperty.Release();

	return hr;
}

HRESULT DShowVideoCapture::EnumCaptureDevices()
{
	HRESULT hr = S_OK;

	int index = 0;
	CComPtr<ICreateDevEnum> pDevEnum = NULL;
	CComPtr<IEnumMoniker> pDevEnumMoniker = NULL;
	CComPtr<IMoniker> pM = NULL;
	CComPtr<IPropertyBag> pProperty = NULL;

	CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum,
		NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum));
	CHECK_HR(hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pDevEnumMoniker, 0));

	pDevEnumMoniker->Reset();
	while (hr = pDevEnumMoniker->Next(1, &pM, NULL) == S_OK){
		VARIANT name;
		VARIANT path;
		VariantInit(&name);
		VariantInit(&path);
		if (pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProperty) == S_OK){
			pProperty->Read(TEXT("FriendlyName"), &name, 0);
			pProperty->Read(TEXT("DevicePath"), &path, 0);

			CAMERADESC desc(name, path, index);
			mCameraList.push_back(desc);
			pProperty.Release();
		}
		VariantClear(&name);
		VariantClear(&path);
		index++;
		pM.Release();
	}

done:
	pDevEnumMoniker.Release();
	pDevEnum.Release();
	return hr;
}