#include "stdafx.h"


#include "logger.h"

#include "dshowVideoCapture.h"
#include "dshowutil.h"

using namespace ATL;

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

bool DShowVideoCapture::Runing()
{
	HRESULT hr = E_FAIL;
	OAFilterState status = State_Stopped;
	if (mMediaControl)
		hr = mMediaControl->GetState(INFINITE, &status);
	if (status == State_Stopped || FAILED(hr)){
		return true;
	}else{
		return false;
	}
}

HRESULT DShowVideoCapture::Start()
{
	return mMediaControl->Run();
}

HRESULT DShowVideoCapture::Stop()
{
	return mMediaControl->Stop();
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

	if (mGraph){
		CHECK_HR(mGraph->EnumFilters(&pFilterEnum));
		while (pFilterEnum->Next(1, &pFilter, NULL) == S_OK){
			mGraph->RemoveFilter(pFilter);
			pFilter.Release();
			pFilterEnum->Reset();
		}
	}

done:
	return hr;
}

HRESULT DShowVideoCapture::ReleaseDShowInterfaces()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(mCaptureStatus);
	SAFE_RELEASE(mMediaEventEx);
	SAFE_RELEASE(mMediaControl);
	SAFE_RELEASE(mVideoWindow);
	SAFE_RELEASE(mGrabberFiler);
	SAFE_RELEASE(mGrabber);
	SAFE_RELEASE(mGraphBuiler);
	SAFE_RELEASE(mGraph);

	return hr;
}

HRESULT DShowVideoCapture::BuildGraph(int index, int width, int height, int fps)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT DShowVideoCapture::GetDevices(std::vector<const TCHAR*> cameNames)
{
	HRESULT hr = E_FAIL;
	if (camlist.size() != 0){
		std::list<CAMERADESC>::iterator it = camlist.begin();
		for (; it != camlist.end(); it++){
			cameNames.push_back(it->name.c_str());
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT DShowVideoCapture::findFilterByIndex(int index)
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> pDevEnum = NULL;
	CComPtr<IEnumMoniker> pDevEnumMoniker = NULL;
	CComPtr<IMoniker> pM = NULL;
	CComPtr<IPropertyBag> pProperty = NULL;


	CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum));
	CHECK_HR(hr = pDevEnum->QueryInterface(CLSID_VideoInputDeviceCategory, (void**)&pDevEnumMoniker));

	pDevEnumMoniker->Reset();
	if (index != 0){
		pDevEnumMoniker->Skip(index);
	}

	CHECK_HR(hr = pDevEnumMoniker->Next(1, &pM, NULL));
	CHECK_HR (hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProperty));

done:
	return hr;
}

HRESULT DShowVideoCapture::EnumCaptureDevices()
{
	HRESULT hr = S_OK;

	CComPtr<ICreateDevEnum> pDevEnum = NULL;
	CComPtr<IEnumMoniker> pDevEnumMoniker = NULL;
	CComPtr<IMoniker> pM = NULL;
	CComPtr<IPropertyBag> pProperty = NULL;

	CHECK_HR(hr = CoCreateInstance(CLSID_SystemDeviceEnum,
		NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum));
	CHECK_HR(hr = pDevEnum->QueryInterface(CLSID_VideoInputDeviceCategory, (void**)&pDevEnumMoniker));

	while (hr = pDevEnumMoniker->Next(1, &pM, NULL) == S_OK){
		VARIANT name;
		VARIANT path;
		VARIANT clsid;
		VariantInit(&name);
		VariantInit(&path);
		VariantInit(&clsid);
		if (pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProperty) == S_OK){
			pProperty->Read(TEXT("FriendlyName"), &name, 0);
			pProperty->Read(TEXT("Path"), &path, 0);
			pProperty->Read(TEXT("CLSID"), &clsid, 0);

			CAMERADESC desc(name, path, clsid);
			camlist.push_back(desc);
			pProperty.Release();
		}
		VariantClear(&name);
		VariantClear(&path);
		VariantClear(&clsid);
		pM.Release();
	}

done:
	pDevEnum.Release();
	pDevEnumMoniker.Release();
	return hr;
}