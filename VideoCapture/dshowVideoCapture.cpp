#include "stdafx.h"

#include "logger.h"
#include "dshowVideoCapture.h"
#include "dshowutil.h"

#define CAPTURE_FILTER_NAME_STR (TEXT("Capture Filter"))
#define RENDER_FILTER_NAME_STR (TEXT("Null Renderer"))
#define GRABBER_FILTER_NAME_STR (TEXT("SampleGrabber"))
#define JPEGDEC_FILTER_NAME_STR (TEXT("MJPEG Decompressor"))

/*
Reference :https://msdn.microsoft.com/zh-cn/library/windows/desktop/dd391027(v=vs.85).aspx
MEDIASUBTYPE_YV12 YV12 4:2:0 Planar 8
MEDIASUBTYPE_I420 I420 4:2:0 Planar 8
MEDIASUBTYPE_NV12 NV12 4:2:0 Planar 8
MEDIASUBTYPE_IYUV IYUV 4:2:0 Planar 8

MEDIASUBTYPE_IMC1 IMC1 4:2:0 Planar 8
MEDIASUBTYPE_IMC3 IMC2 4:2:0 Planar 8
MEDIASUBTYPE_IMC2 IMC3 4:2:0 Planar 8
MEDIASUBTYPE_IMC4 IMC4 4:2:0 Planar 8

MEDIASUBTYPE_YUY2 YUY2 4:2:2 Packed 8
MEDIASUBTYPE_UYVY UYVY 4:2:2 Packed 8
MEDIASUBTYPE_YVYU YVYU 4:2:2 Packed 8

MEDIASUBTYPE_AYUV AYUV 4:4:4 Packed 8
*/
#ifndef MEDIASUBTYPE_I420
const GUID MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
#endif

const FRAMEFORAMTINFO videoFormatTable[] = {
	/* YUV 420 */
	{ MEDIASUBTYPE_I420, PIXEL_FORMAT_I420, 1 << 15, 8, 3, { 0, 2, 2, 0} },
	{ MEDIASUBTYPE_YV12, PIXEL_FORMAT_YV12, 1 << 15, 8, 2, { 0, 1, 0, 0 } },
	{ MEDIASUBTYPE_NV12, PIXEL_FORMAT_NV12, 1 << 15, 8, 2, { 0, 1, 0, 0 } },
	{ MEDIASUBTYPE_IYUV, PIXEL_FORMAT_IYUV, 1 << 14, 8, 2, { 0, 1, 0, 0 } },

	/* MJPEG*/
	{ MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG, 1 << 14, 1, 1 },

	/* RGB */
	{ MEDIASUBTYPE_RGB565, PIXEL_FORMAT_RGB565, 1 << 13, 16, 1 },
	{ MEDIASUBTYPE_RGB555, PIXEL_FORMAT_RGB555, 1 << 13, 16, 1 },
	{ MEDIASUBTYPE_RGB24, PIXEL_FORMAT_RGB24, 1 << 12, 24, 1 },
	{ MEDIASUBTYPE_RGB32, PIXEL_FORMAT_RGB32, 1 << 14, 32, 1 },
	{ MEDIASUBTYPE_ARGB32, PIXEL_FORMAT_ARGB, 1 << 14, 32, 1 },

	/* YUV 4:2:2 */
	{ MEDIASUBTYPE_YUYV, PIXEL_FORMAT_YUYV, 1 << 13, 16, 1, { 0, 0, 0, 0 } }, /*422 16bits per pixel top-down image, YUYV packet */
	{ MEDIASUBTYPE_YVU9, PIXEL_FORMAT_YVU9, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y411, PIXEL_FORMAT_Y411, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y41P, PIXEL_FORMAT_Y41P, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_YVYU, PIXEL_FORMAT_YVYU, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY, 1 << 13, 16, 1, { 0, 0, 0, 0 } },
	{ MEDIASUBTYPE_Y211, PIXEL_FORMAT_Y211, 1 << 13, 16, 2 },
	{ MEDIASUBTYPE_NV11, PIXEL_FORMAT_NV11, 1 << 13, 16, 2 },
	{ MEDIASUBTYPE_420O, PIXEL_FORMAT_420O, 1 << 13, 16, 3 },

	/* YUV 4:4:4 */
	{ MEDIASUBTYPE_AYUV, PIXEL_FORMAT_AYUV, 1 << 10, 32, 1}, /*AYUV4:4:4 Packed 8*/
};

//using namespace ATL;

DShowVideoCapture::DShowVideoCapture()
	: mGraph(NULL)
	, mMediaControl(NULL)
	, mMediaEventEx(NULL)
	, mDropFrameStatus(NULL)
	, mCaptureFilter(NULL)
	, mGrabberFiler(NULL)
	, mVideoGrabber(NULL)
	, mcb(NULL)
	, mWorkFrameInfo(NULL)
	, mGraphRegisterHandler(0)
{
	
}

DShowVideoCapture::~DShowVideoCapture()
{
	internel_log(Info, "fps %f", mFpsStats.Frequency());
	ReleaseDShowInterfaces();
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

HRESULT DShowVideoCapture::Start(OPEN_DEVICE_PARAM &params)
{
	HRESULT hr = S_OK;
	ASSERT(mMediaControl);

	mWorkParams = params;
	if (!mBufferManager.Reset(RES1080P, 10)){
		hr = E_OUTOFMEMORY;
		goto done;
	}

	CHECK_HR(hr = BuildGraph());

	CHECK_HR(hr = mVideoGrabber->SetBufferSamples(FALSE));
	CHECK_HR(hr = mVideoGrabber->SetCallback(this, 0));
	CHECK_HR(hr = mVideoGrabber->SetOneShot(FALSE));

	// update work parameters 
	CHECK_HR(hr = mVideoGrabber->GetConnectedMediaType(&mWorkMediaType));
	mWorkParams.width = mWorkMediaType.BitmapHeader()->biWidth;
	mWorkParams.height = mWorkMediaType.BitmapHeader()->biHeight;
	mWorkParams.fps = RefTimeToFramesPerSec(mWorkMediaType.AvgReferenceTime());
	mWorkParams.pixelFormatInFourCC = mWorkMediaType.subtype.Data1;
	mWorkFrameInfo = GetFrameInfoByFourCC(mWorkMediaType.subtype.Data1);
	params = mWorkParams;
	
	do{
		Sleep(100);
		hr = mMediaControl->Run();
		if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)){
			break;
		}
	} while (hr != S_OK);

	if (mDropFrameStatus){
		mDropFrameStatus->GetNumDropped(&mDropFrames);
		mDropFrameStatus->GetNumNotDropped(&mCapFrames);
	}

done:
	if (FAILED(hr)){
		Stop();
	}
	ShowDShowError(hr);

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

	while(FAILED(mMediaControl->Stop()));
	if (mGraphRegisterHandler)
		RemoveGraphFromRot(mGraphRegisterHandler);
	mGraphRegisterHandler = 0;
	RemoveFiltersFromGraph();
	mWorkFrameInfo = NULL;

	return S_OK;
}

inline const FRAMEFORAMTINFO* DShowVideoCapture::GetFrameInfoByFourCC(DWORD pixelFormat)
{
	int32_t bitsPerFixel = 0;
	const FRAMEFORAMTINFO *info = NULL;
	for (int i = 0; i < ARRAYSIZE(videoFormatTable); i++){
		if (videoFormatTable[i].pixelFormatInFourCC == pixelFormat){
			info = &videoFormatTable[i];
			break;
		}
	}
	
	return info;
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
	if (FAILED(hr) || (desc.ptsStart == 0)){
		desc.ptsStart = timeGetTime(); // convert to 100ns
		desc.ptsEnd = desc.ptsStart + FramesPerSecToRefTime(mWorkParams.fps) / 10000;
	}else{
		desc.ptsStart = timeGetTime(); // convert to 100ns
		if (hr == VFW_S_NO_STOP_TIME)
			desc.ptsEnd = desc.ptsStart + FramesPerSecToRefTime(mWorkParams.fps) / 10000;
	}

	desc.width = mWorkParams.width;
	desc.height = mWorkParams.height;
	desc.pixelFormatInFourCC = mWorkMediaType.subtype.Data1;
	desc.lineSize = mWorkParams.width * mWorkFrameInfo->bytePerPixel / 8;
	desc.planeCnt = mWorkFrameInfo->planeCnt;
	memcpy(&desc.planeStride, &mWorkFrameInfo->planeStride, sizeof(mWorkFrameInfo->planeStride));

	mBufferManager.FillFrame(desc);

done:
	mFpsStats.AppendSample(1);

	return hr;
}

BOOL DShowVideoCapture::GetFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mBufferManager.LockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

BOOL DShowVideoCapture::ReleaseFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mBufferManager.UnlockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
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
	mVideoGrabber->SetCallback(NULL, 0);
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
	SAFE_RELEASE(mVideoGrabber);
	SAFE_RELEASE(mCaptureFilter);
	SAFE_RELEASE(mDropFrameStatus);
	SAFE_RELEASE(mMediaEventEx);
	SAFE_RELEASE(mMediaControl);
	SAFE_RELEASE(mGrabberFiler);
	SAFE_RELEASE(mGraph);
	SAFE_RELEASE(mGraphBuiler);

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
	CComPtr<IMediaFilter> pMediaFilter;

	CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&mGraph));

	CHECK_HR(hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&mGraphBuiler));

	CHECK_HR(hr = mGraphBuiler->SetFiltergraph(mGraph));

	CHECK_HR(hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
		CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&mVideoGrabber));
	CHECK_HR(hr = mVideoGrabber->QueryInterface(IID_IBaseFilter, (void**)&mGrabberFiler));

	CHECK_HR(hr = mGraph->QueryInterface(IID_IMediaControl, (void**)&mMediaControl));
	CHECK_HR(hr = mGraph->QueryInterface(IID_IMediaEventEx, (void**)&mMediaEventEx));
	CHECK_HR(hr = mGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter));
	// not set sync clock
	CHECK_HR(hr = pMediaFilter->SetSyncSource(NULL));
	
done:
	ShowDShowError(hr);
	pMediaFilter.Release();
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
	CMediaType mediaTypeFound;
	CComPtr<IAMStreamConfig> pStreamConfig = NULL;

	CHECK_HR(hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderFilter));

	CHECK_HR(hr = FindCaptureFilterByIndex(mWorkParams.index, mCaptureFilter));
	ASSERT(mCaptureFilter);

	CHECK_HR(hr = mGraph->AddFilter(mCaptureFilter, mCameraList[mWorkParams.index].name.c_str()));
	CHECK_HR(hr = mGraph->AddFilter(mGrabberFiler, GRABBER_FILTER_NAME_STR));
	CHECK_HR(hr = mGraph->AddFilter(pNullRenderFilter, RENDER_FILTER_NAME_STR));
#if 0
	CHECK_HR(hr = FindPinByCategory(mCaptureFilter, PIN_CATEGORY_CAPTURE, PINDIR_OUTPUT, &pCaptureOutPin));
	CHECK_HR(hr = FindMediaTypeInPinOrStreamConfig(pCaptureOutPin, mediaTypeFound, NULL));

	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_OUTPUT, &pGrabberOutPin));
	CHECK_HR(hr = FindUnconnectedPin(mGrabberFiler, PINDIR_INPUT, &pGrabberInPin));
	CHECK_HR(hr = FindUnconnectedPin(pNullRenderFilter, PINDIR_INPUT, &pNullRenderInPin));

	if (mediaTypeFound.subTypeEqual(MEDIASUBTYPE_MJPG)){
		CComPtr<IPin> pJpegDecInPin = NULL;
		CComPtr<IPin> pJpegDecOutPin = NULL;
		CHECK_HR(hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pJpegDecFilter));
		CHECK_HR(hr = mGraph->AddFilter(pJpegDecFilter, JPEGDEC_FILTER_NAME_STR));
		CHECK_HR(hr = FindUnconnectedPin(pJpegDecFilter, PINDIR_INPUT, &pJpegDecInPin));
		CHECK_HR(hr = FindUnconnectedPin(pJpegDecFilter, PINDIR_OUTPUT, &pJpegDecOutPin));
		CHECK_HR(hr = mGraph->Connect(pCaptureOutPin, pJpegDecInPin));
		CHECK_HR(hr = mGraph->Connect(pJpegDecOutPin, pGrabberInPin));
		CHECK_HR(hr = mGraph->Connect(pGrabberOutPin, pNullRenderInPin));
		mediaTypeFound.majortype = MEDIATYPE_Video;
		mediaTypeFound.subtype = MEDIASUBTYPE_RGB32;

	}else{
		CHECK_HR(hr = mGraph->Connect(pCaptureOutPin, pGrabberInPin));
		CHECK_HR(hr = mGraph->Connect(pGrabberOutPin, pNullRenderInPin));
	}

	CHECK_HR(hr = mVideoGrabber->SetMediaType(&mediaTypeFound));
#else
	CHECK_HR(hr = mGraphBuiler->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mCaptureFilter, IID_IAMStreamConfig, (void**)&pStreamConfig));
	CHECK_HR(hr = FindMediaTypeInPinOrStreamConfig(pCaptureOutPin, mediaTypeFound, pStreamConfig));
	if (mediaTypeFound.subTypeEqual(MEDIASUBTYPE_MJPG)){
		CHECK_HR(hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pJpegDecFilter));
		CHECK_HR(hr = mGraph->AddFilter(pJpegDecFilter, JPEGDEC_FILTER_NAME_STR));
		mediaTypeFound.majortype = MEDIATYPE_Video;
		mediaTypeFound.subtype = MEDIASUBTYPE_RGB32;
	}

	CHECK_HR(hr = mVideoGrabber->SetMediaType(&mediaTypeFound));
	CHECK_HR(hr = mGraphBuiler->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mCaptureFilter, mGrabberFiler, pNullRenderFilter));
#endif

	CHECK_HR(hr = AddGraphToRot(mGraph, &mGraphRegisterHandler));

done:
	TCHAR Buffer[MAX_PATH];
	if (GetCurrentDirectory(MAX_PATH, Buffer) != 0){
		STRING path(Buffer);
		path.append(TEXT("\\my.grf"));
		SaveGraphFile(mGraph, (TCHAR*)path.c_str());
	}
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
	for (int i = 0; i < ARRAYSIZE(videoFormatTable); i++){
		if (mediaType.subTypeEqual(videoFormatTable[i].subtype)){
			bility.Priority = videoFormatTable[i].priority;
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

/*
 * resolution is priority
 */
HRESULT DShowVideoCapture::FindMediaTypeInPinOrStreamConfig(CComPtr<IPin> &pOutPin, CMediaType &requestMediaType,  IAMStreamConfig* pStreamConfig)
{
	HRESULT hr = S_OK;
	HRESULT hrRet = E_FAIL;
	CComPtr<IAMStreamConfig> pConfig = NULL;
	int cfgCnt = 0;
	int cfgSize = 0;
	std::list<FRAMEABILITY> supportFrameFormatList;

	if (!pStreamConfig){
		CHECK_HR(hr = pOutPin->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig));
	}
	else{
		pStreamConfig->AddRef();
		pConfig = pStreamConfig;
	}

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
					}else if (bility.ImageSize.cx * mWorkParams.height == bility.ImageSize.cy * mWorkParams.width){
						bility.Ability |= FRAMEABILITY::SU_RES_RATIO;
					}else if (bility.ImageSize.cx > mWorkParams.width && bility.ImageSize.cy > mWorkParams.height){
						bility.Ability |= FRAMEABILITY::SU_RES_LARGE;
					}else if (bility.ImageSize.cx * bility.ImageSize.cy > mWorkParams.width * mWorkParams.height){
						bility.Ability |= FRAMEABILITY::SU_RES_LARGE_INAREA;
					}else{
						continue;
					}

					REFERENCE_TIME frameInterval = FramesPerSecToRefTime(mWorkParams.fps);
					if (frameInterval >= bility.MinFrameInterval || frameInterval <= bility.MaxFrameInterval){
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
		int64_t largestAbility = 0;
		FRAMEABILITY ability;
		std::list<FRAMEABILITY>::iterator it = supportFrameFormatList.begin();
		for (; it != supportFrameFormatList.end(); it++){
			int64_t distance = it->Ability * it->Priority;
			if (distance > largestAbility){
				ability = *it;
				largestAbility = distance;
			}
		}

		REFERENCE_TIME frameInterval = FramesPerSecToRefTime(mWorkParams.fps);
		requestMediaType = ability.MediaType;
		if (frameInterval > ability.MaxFrameInterval){
			frameInterval = ability.MaxFrameInterval;
		}
		if (frameInterval < ability.MinFrameInterval){
			frameInterval = ability.MinFrameInterval;
		}
		requestMediaType.SetAvgReferenceTime(frameInterval);

		CHECK_HR(hr = pConfig->SetFormat(&requestMediaType));
		
		hrRet = S_OK;
	}

done:
	ShowDShowError(hr);
	pConfig.Release();

	return hrRet;
}

HRESULT DShowVideoCapture::GetFilterFriendlyName(IBaseFilter * &filter, STRING name)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT DShowVideoCapture::FindCaptureFilterByIndex(int index, IBaseFilter * &filter)
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