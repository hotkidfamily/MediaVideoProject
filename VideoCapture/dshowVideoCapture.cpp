#include "stdafx.h"

#include "logger.h"

#include "dshowVideoCapture.h"
#include "dshowutil.h"

#define CAPTURE_FILTER_NAME_STR (TEXT("Capture Filter"))
#define RENDER_FILTER_NAME_STR (TEXT("NULL Render Filter"))
#define GRABBER_FILTER_NAME_STR (TEXT("Grabber Filter"))

using namespace ATL;

DShowVideoCapture::DShowVideoCapture()
{
}

DShowVideoCapture::~DShowVideoCapture()
{
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
	if(FAILED(hr)){
		internel_log(Error, "Get dshow interface error: %d", hr);
	}

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

	return bRet;
}

HRESULT DShowVideoCapture::Start(OPEN_DEVICE_PARAM params)
{
	ASSERT(mMediaControl);

	workParams = params;

	mRender = new VMR7();
	ASSERT(mRender);

	BuildGraph();

	return mMediaControl->Run();
}

HRESULT DShowVideoCapture::Stop()
{
	ASSERT(mMediaControl);

	SAFE_DELETE(mRender);

	return mMediaControl->Stop();
}

HRESULT DShowVideoCapture::SampleCB(double SampleTime, IMediaSample *pSample)
{
	FRAME_DESC desc;
	long dataLength = 0;
	HRESULT  hr = S_OK;

	ASSERT(mcb);

	CHECK_HR(hr = pSample->GetPointer(&desc.dataPtr));
	dataLength = pSample->GetActualDataLength();
	hr = pSample->GetMediaTime(&desc.frameStartIdx, &desc.frameEndIdx);
	hr = pSample->GetTime(&desc.ptsStart, &desc.ptsEnd);

	if (FAILED(hr)){
		desc.ptsStart = timeGetTime();
		desc.ptsEnd = desc.ptsStart + RefTimeToMsec(workParams.avgFrameIntervalInNs);
	}

done:
	if (FAILED(hr)){
		// drop frame
		mcb->OnFrame(desc);
	}

	return hr;
}

HRESULT DShowVideoCapture::Repaint(HDC hdc)
{
	return mRender->Repaint(workParams.parentWindow, hdc);
}

HRESULT DShowVideoCapture::UpdateVideoWindow(HWND hWnd, const LPRECT prc)
{
	return mRender->UpdateVideoWindow(hWnd, prc);
}

HRESULT DShowVideoCapture::ShowCapturePropertyWindow()
{
	HRESULT hr = S_OK;
	ASSERT(mGraph);
	ASSERT(mCaptureFilter);

	hr = ShowFilterPropertyPage(mCaptureFilter, workParams.parentWindow);

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

	SAFE_RELEASE(mCaptureStatus);
	SAFE_RELEASE(mMediaEventEx);
	SAFE_RELEASE(mMediaControl);
	SAFE_RELEASE(mGrabberFiler);
	SAFE_RELEASE(mGrabber);
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
	CMediaType mediaType;

	CHECK_HR(CoCreateInstance(CLSID_NullRenderer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderFilter));

	CHECK_HR(hr = findFilterByIndex(workParams.index, mCaptureFilter));
	CHECK_HR(hr = mGraph->AddFilter(mCaptureFilter, CAPTURE_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(mGrabberFiler, GRABBER_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(pNullRenderFilter, RENDER_FILTER_NAME_STR));
	mGraphBuiler->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mCaptureFilter, mGrabberFiler, pNullRenderFilter);

	mRender->AddToGraph(mGraph, workParams.parentWindow);
	mRender->FinalizeGraph(mGraph);
	
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_RGB24;

	mGrabber->SetMediaType(&mediaType);
	mGrabber->SetCallback(this, 0);
	mGrabber->SetOneShot(FALSE);

done:
	return hr;
}

HRESULT DShowVideoCapture::GetDevicesName(VECT &cameNames)
{
	HRESULT hr = E_FAIL;

	if (camlist.size() != 0){
		CAMERALIST::iterator it = camlist.begin();
		for (; it != camlist.end(); it++){
			cameNames.push_back(it->name.c_str());
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT DShowVideoCapture::findFilterByIndex(int index, IBaseFilter * &filter)
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

			if (camlist[index] == dev){
				hr = pM->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&filter);
				break;
			}
		}
		VariantClear(&name);
		VariantClear(&path);
		pM.Release();
	}

done:
	if (FAILED(hr)){
		// record
	}

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
			camlist.push_back(desc);
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