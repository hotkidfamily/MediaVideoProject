#include "stdafx.h"

#include "logger.h"
#include "dshowVideoCapture.h"
#include "dshowutil.h"

#define CAPTURE_FILTER_NAME_STR (TEXT("Capture Filter"))
#define RENDER_FILTER_NAME_STR (TEXT("NULL Render Filter"))
#define GRABBER_FILTER_NAME_STR (TEXT("Grabber Filter"))
#define JPEGDEC_FILTER_NAME_STR (TEXT("JPEG DEC"))

typedef struct tagFrameFormatInfo{
	GUID subtype;
	DWORD pixelFormatInFourCC;
	int proirity;
}FRAMEFORAMTINFO;

FRAMEFORAMTINFO dshowSupportVideoFormatTable[] = {
		{ MEDIASUBTYPE_YUYV, 'VYUY', 1 },
		{ MEDIASUBTYPE_IYUV, 'VYUI', 1 },
		{ MEDIASUBTYPE_YVU9, '9UVY', 1 },
		{ MEDIASUBTYPE_Y411, '114Y', 1 },
		{ MEDIASUBTYPE_Y41P, 'P14Y', 1 },
		{ MEDIASUBTYPE_YUY2, '2YUY', 1 },
		{ MEDIASUBTYPE_YVYU, 'UYVY', 1 },
		{ MEDIASUBTYPE_UYVY, 'YVYU', 1 },
		{ MEDIASUBTYPE_Y211, '112Y', 1 },
		{ MEDIASUBTYPE_MJPG, 'GPJM', 1 },
		{ MEDIASUBTYPE_RGB565, 0xe436eb7b, 0},
		{ MEDIASUBTYPE_RGB555, 0xe436eb7c, 0 },
		{ MEDIASUBTYPE_RGB24, 0xe436eb7d, 0 },
		{ MEDIASUBTYPE_RGB32, 0xe436eb7e, 0 },
		{ MEDIASUBTYPE_ARGB32, 0x773c9ac0, 1},
		{ MEDIASUBTYPE_YV12, '21VY', 0 },
		{ MEDIASUBTYPE_NV12, '21VN', 0 },
		{ MEDIASUBTYPE_NV11, '11VN', 2 },
		{ MEDIASUBTYPE_420O, 'O024', 0 }
};

//using namespace ATL;

DShowVideoCapture::DShowVideoCapture()
	: mGraph(NULL)
	, mGraphBuiler(NULL)
	, mMediaControl(NULL)
	, mMediaEventEx(NULL)
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

BOOL DShowVideoCapture::Runing()
{
	HRESULT hr = S_OK;
	BOOL bRet = TRUE;
	OAFilterState status = State_Stopped;

	ASSERT(mMediaControl);
	
	hr = mMediaControl->GetState(INFINITE, &status);
	if (status == State_Stopped || FAILED(hr)){
		bRet = FALSE;
	}
	ShowDShowError(hr);
	return bRet;
}

HRESULT DShowVideoCapture::Start(OPEN_DEVICE_PARAM params)
{
	HRESULT hr = S_OK;
	ASSERT(mMediaControl);

	mWorkParams = params;

	BuildGraph();

	while (hr = mMediaControl->Run() != S_OK){
		Sleep(100);
	}
	
	mGrabber->GetConnectedMediaType(&mWorkMediaType);

	if (mDropFrameStatus){
		mDropFrameStatus->GetNumDropped(&mDropFrames);
		mDropFrameStatus->GetNumNotDropped(&mCapFrames);
	}

	ShowDShowError(hr);

	SaveGraphFile(mGraph, TEXT("d:\\my.grf"));

	return hr;
}

HRESULT DShowVideoCapture::Stop()
{
	ASSERT(mMediaControl);

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
		desc.ptsEnd = desc.ptsStart + FramesPerSecToRefTime(mWorkParams.fps);
	}

	desc.width = mWorkParams.width;
	desc.height = mWorkParams.height;
	desc.pixelFormatInFourCC = mWorkMediaType.subtype.Data1;

	mcb->OnFrame(desc);

done:
	mFpsStats.appendDataSize(1);

	return hr;
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

	return hr;
}


HRESULT DShowVideoCapture::SaveGraphFile(IGraphBuilder *pGraph, TCHAR *wszPath)
{
	const WCHAR wszStreamName[] = L"ActiveMovieGraph";
	HRESULT hr;

	IStorage *pStorage = NULL;
	hr = StgCreateDocfile(
		wszPath,
		STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStorage);
	if (FAILED(hr)){
		return hr;
	}

	IStream *pStream;
	hr = pStorage->CreateStream(
		wszStreamName,
		STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStream);
	if (FAILED(hr)){
		pStorage->Release();
		return hr;
	}

	IPersistStream *pPersist = NULL;
	pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
	hr = pPersist->Save(pStream, TRUE);
	pStream->Release();
	pPersist->Release();
	if (SUCCEEDED(hr)){
		hr = pStorage->Commit(STGC_DEFAULT);
	}
	pStorage->Release();
	return hr;
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

HRESULT DShowVideoCapture::BuildGraph()
{
	HRESULT hr = E_FAIL;
	CComPtr<IBaseFilter> pNullRenderFilter = NULL;
	CComPtr<IBaseFilter> pJpegDecFilter = NULL;
	CComPtr<IPin> pCaptureOutPin = NULL;
	CComPtr<IPin> pNullRenderInPin = NULL;
	CComPtr<IPin> pGrabberInPin = NULL;
	CComPtr<IPin> pGrabberOutPin = NULL;

	CMediaType mediaType;
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_NULL;
	CHECK_HR(hr = mGrabber->SetMediaType(&mediaType));

	CHECK_HR(hr = CoCreateInstance(CLSID_NullRenderer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderFilter));

	CHECK_HR(hr = FindFilterByIndex(mWorkParams.index, mCaptureFilter));
	ASSERT(mCaptureFilter);

	CHECK_HR(hr = mGraph->AddFilter(mCaptureFilter, CAPTURE_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(mGrabberFiler, GRABBER_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(pNullRenderFilter, RENDER_FILTER_NAME_STR));

	//CHECK_HR(hr = FindSultablePin(pCaptureOutPin));
	CHECK_HR(hr = FindPinByCategory(mCaptureFilter, PIN_CATEGORY_CAPTURE, PINDIR_OUTPUT, &pCaptureOutPin));
	FindMediaTypeInPin(pCaptureOutPin);

	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_OUTPUT, &pGrabberOutPin));
	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_INPUT, &pGrabberInPin));
	CHECK_HR(hr = FindUnconnectedPin(pNullRenderFilter, PINDIR_INPUT, &pNullRenderInPin));

	if (mWorkMediaType.subTypeEqual(MEDIASUBTYPE_MJPG)){
		CComPtr<IPin> pJpegDecInPin = NULL;
		CComPtr<IPin> pJpegDecOutPin = NULL;
		CHECK_HR(hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pJpegDecFilter));
		CHECK_HR(hr = mGraph->AddFilter(pJpegDecFilter, JPEGDEC_FILTER_NAME_STR));
		CHECK_HR(hr = FindUnconnectedPin(pJpegDecFilter, PINDIR_INPUT, &pJpegDecInPin));
		CHECK_HR(hr = FindUnconnectedPin(pJpegDecFilter, PINDIR_OUTPUT, &pJpegDecOutPin));
		CHECK_HR(hr = mGraph->Connect(pCaptureOutPin, pJpegDecInPin));
		CHECK_HR(hr = mGraph->Connect(pJpegDecOutPin, pGrabberInPin));
		CHECK_HR(hr = mGraph->Connect(pGrabberOutPin, pNullRenderInPin));
		mWorkMediaType.SetSubtype(&MEDIASUBTYPE_RGB32);
	}else{
		CHECK_HR(hr = mGraph->Connect(pCaptureOutPin, pGrabberInPin));
		CHECK_HR(hr = mGraph->Connect(pGrabberOutPin, pNullRenderInPin));
	}

	CHECK_HR(hr = mGrabber->SetCallback(this, 0));
	CHECK_HR(hr = mGrabber->SetOneShot(FALSE));

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

inline BOOL DShowVideoCapture::IsFormatSupport(CMediaType &mediaType, FRAMEABILITY & bility)
{
	BOOL bRet = FALSE;
	for (int i = 0; i < ARRAYSIZE(dshowSupportVideoFormatTable); i++){
		if (mediaType.subTypeEqual(dshowSupportVideoFormatTable[i].subtype)){
			bility.Proirity = dshowSupportVideoFormatTable[i].proirity;
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

/*
 * resolution is priority
 */
HRESULT DShowVideoCapture::FindMediaTypeInPin(CComPtr<IPin> &pOutPin)
{
	HRESULT hr = S_OK;
	HRESULT hrRet = E_FAIL;
	CComPtr<IAMStreamConfig> pConfig = NULL;
	int cfgCnt = 0;
	int cfgSize = 0;
	std::list<FRAMEABILITY> supportFrameFormatList;
	std::list<FRAMEABILITY> suitableFrameFromatList;

	CHECK_HR(hr = pOutPin->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig));
	CHECK_HR(hr = pConfig->GetNumberOfCapabilities(&cfgCnt, &cfgSize));

	/*
	 * Get all output format
	 */
	for (int i = 0; i < cfgCnt; i++){
		CMediaType *mediaType = NULL;
		VIDEO_STREAM_CONFIG_CAPS caps;
		if ((hr = pConfig->GetStreamCaps(i, (AM_MEDIA_TYPE**)&mediaType, (BYTE*)&caps)) == S_OK){
			if (mediaType->isVideoInfoHeader()){ // sample only support format_videoinfo
				BITMAPINFOHEADER *bmp = mediaType->BitmapHeader();
				FRAMEABILITY bility;
				if (IsFormatSupport(*mediaType, bility)){
					bility.MaxFrameInterval = caps.MaxFrameInterval;
					bility.MinFrameInterval = caps.MinFrameInterval;
					bility.ImageSize.cx = abs(bmp->biWidth);
					bility.ImageSize.cy = abs(bmp->biHeight);
					bility.MediaType = *mediaType;

					if (bility.ImageSize.cx == mWorkParams.width && bility.ImageSize.cy == mWorkParams.height){
						bility.Ability |= FRAMEABILITY::SU_RES;
					}
					else if (bility.ImageSize.cx > mWorkParams.width && bility.ImageSize.cy > mWorkParams.height){
						bility.Ability |= FRAMEABILITY::SU_RES_LARGE;
					}
					else if (bility.ImageSize.cx * bility.ImageSize.cy > mWorkParams.width * mWorkParams.height){
						bility.Ability |= FRAMEABILITY::SU_RES_LARGE_INAREA;
					}

					if (bility.ImageSize.cx * mWorkParams.height == bility.ImageSize.cy * mWorkParams.width){
						bility.Ability |= FRAMEABILITY::SU_RES_RATIO;
					}

					REFERENCE_TIME frameInterval = FramesPerSecToRefTime(mWorkParams.fps);
					if (frameInterval < bility.MinFrameInterval || frameInterval > bility.MaxFrameInterval){
						bility.Ability |= FRAMEABILITY::SU_FPS;
					}

					if (bility.Ability)
						supportFrameFormatList.push_back(bility);
				}
			}
		}
	}

	/*
	 * select support format
	 */
	if (supportFrameFormatList.size()){
		int32_t largeAbility = 0;
		FRAMEABILITY ability;
		std::list<FRAMEABILITY>::iterator it = supportFrameFormatList.begin();
		for (; it != supportFrameFormatList.end(); it++){
			if (it->Ability > largeAbility){
				ability = *it;
				largeAbility = it->Ability;
			}
		}

		REFERENCE_TIME frameInterval = FramesPerSecToRefTime(mWorkParams.fps);
		mWorkMediaType = ability.MediaType;
		if (frameInterval > ability.MaxFrameInterval){
			frameInterval = ability.MaxFrameInterval;
		}
		if (frameInterval < ability.MinFrameInterval){
			frameInterval = ability.MinFrameInterval;
		}
		mWorkMediaType.SetAvgReferenceTime(frameInterval);

		CHECK_HR(hr = pConfig->SetFormat(&mWorkMediaType));
	}

	//mWorkMediaType.GUIDtoStr(mWorkMediaType.subtype);

done:
	ShowDShowError(hr);
	pConfig.Release();

	return hrRet;
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
			pProperty.Release();

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