#include "stdafx.h"
#include "ddrawRender.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

DDrawRender::DDrawRender()
	: mDDrawObj(nullptr)
	, mPrimarySurface(nullptr)
	, mCanvasSurface(nullptr)
	, mHwnd(nullptr)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderEvent(nullptr)
	, mDDrawClippper(nullptr)
	, mRenderThreadRuning(FALSE)
	, mSupportVSync(FALSE)
{
}

DDrawRender::~DDrawRender()
{
	DeinitRender();
}

void DDrawRender::FillddPixelFormatFromFourCC(LPDDPIXELFORMAT ddPixelFormat, DWORD dwFourCC)
{
	if (!ddPixelFormat){
		return;
	}
	ZeroMemory(ddPixelFormat, sizeof(DDPIXELFORMAT));
	ddPixelFormat->dwSize = sizeof(DDPIXELFORMAT);

	switch (dwFourCC)
	{
	case PIXEL_FORMAT_RGB24:
		ddPixelFormat->dwFlags = DDPF_RGB;
		ddPixelFormat->dwFourCC = 0;
		ddPixelFormat->dwRGBBitCount = 32;
		ddPixelFormat->dwRBitMask = 0x00ff0000;
		ddPixelFormat->dwGBitMask = 0x0000ff00;
		ddPixelFormat->dwBBitMask = 0x000000ff;
		ddPixelFormat->dwRGBAlphaBitMask = 0x0;
		break;
	case PIXEL_FORMAT_RGB32:
		ddPixelFormat->dwFlags = DDPF_RGB;
		ddPixelFormat->dwFourCC = 0;
		ddPixelFormat->dwRGBBitCount = 32;
		ddPixelFormat->dwRBitMask = 0x00ff0000;
		ddPixelFormat->dwGBitMask = 0x0000ff00;
		ddPixelFormat->dwBBitMask = 0x000000ff;
		ddPixelFormat->dwRGBAlphaBitMask = 0x0;
		break;
	case PIXEL_FORMAT_ARGB:
		ddPixelFormat->dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddPixelFormat->dwFourCC = 0;
		ddPixelFormat->dwRGBBitCount = 32;
		ddPixelFormat->dwRBitMask = 0x00ff0000;
		ddPixelFormat->dwGBitMask = 0x0000ff00;
		ddPixelFormat->dwBBitMask = 0x000000ff;
		ddPixelFormat->dwRGBAlphaBitMask = 0xff000000;
		break;
	case PIXEL_FORMAT_RGB565:
		ddPixelFormat->dwFlags = DDPF_RGB;
		ddPixelFormat->dwFourCC = 0;
		ddPixelFormat->dwRGBBitCount = 16;
		ddPixelFormat->dwRBitMask = 0xF800;
		ddPixelFormat->dwGBitMask = 0x07E0;
		ddPixelFormat->dwBBitMask = 0x001F;
		ddPixelFormat->dwRGBAlphaBitMask = 0x0;
		break;
	case PIXEL_FORMAT_RGB555:
		ddPixelFormat->dwFlags = DDPF_RGB;
		ddPixelFormat->dwFourCC = 0;
		ddPixelFormat->dwRGBBitCount = 16;
		ddPixelFormat->dwRBitMask = 0x7C00;
		ddPixelFormat->dwGBitMask = 0x03E0;
		ddPixelFormat->dwBBitMask = 0x001F;
		ddPixelFormat->dwRGBAlphaBitMask = 0x0;
		break;
	case PIXEL_FORMAT_UYVY:
	case PIXEL_FORMAT_YUY2 :
		ddPixelFormat->dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddPixelFormat->dwFourCC = dwFourCC;
		ddPixelFormat->dwYUVBitCount = 16;
		break;
	case PIXEL_FORMAT_YV12:
		ddPixelFormat->dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddPixelFormat->dwFourCC = dwFourCC;
		ddPixelFormat->dwYUVBitCount = 12;
		break;
	default:
		ddPixelFormat->dwFlags = DDPF_FOURCC | DDPF_YUV;
		ddPixelFormat->dwFourCC = dwFourCC;
		ddPixelFormat->dwRGBBitCount = 12;
		break;
	}
}

HRESULT DDrawRender::CreateSurfaces(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC2 ddsd = {0};
	
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	CHECK_HR(hr = mDDrawObj->CreateSurface(&ddsd, &mPrimarySurface, nullptr));

	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	FillddPixelFormatFromFourCC(&(ddsd.ddpfPixelFormat), pixelFormatInFourCC);

	CHECK_HR(hr = mDDrawObj->CreateSurface(&ddsd, &mCanvasSurface, nullptr));

	mCanvasBpp = ddsd.ddpfPixelFormat.dwRGBBitCount;

done:
	logger(Error, "%s", DXGetErrorStringA(hr));
	return hr;
}

BOOL DDrawRender::InitRender(const RENDERCONFIG &config)
{
	HRESULT hr = DD_OK;
	DDBLTFX ddbltfx = { 0 };
	ZeroMemory(&mHwCaps, sizeof(DDCAPS));
	ZeroMemory(&mHelCaps, sizeof(DDCAPS));

	mHwnd = config.hWnd;

	CHECK_HR(hr = CoInitialize(nullptr));

	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, nullptr, CLSCTX_ALL, IID_IDirectDraw, (void**)&mDDrawObj));
	CHECK_HR(hr = mDDrawObj->Initialize(nullptr));
	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));

	mHwCaps.dwSize = sizeof(DDCAPS);
	mHelCaps.dwSize = sizeof(DDCAPS);
	mDDrawObj->GetCaps(&mHwCaps, &mHelCaps);

	CHECK_HR(hr = CreateSurfaces(config.width, config.height, config.pixelFormat));

	CHECK_HR(hr = mDDrawObj->CreateClipper(0, &mDDrawClippper, nullptr));
	CHECK_HR(hr = mDDrawClippper->SetHWnd(0, mHwnd));
	CHECK_HR(hr = mPrimarySurface->SetClipper(mDDrawClippper));

	/* clear screen */
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = RGB(0, 0, 0);
	hr = mPrimarySurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	if (hr == DDERR_SURFACELOST){
		//Restore surface
	}

	mLastPts = 0;
	mLastTime = 0;
	mScreenSizeInPixel.cx = config.width;
	mScreenSizeInPixel.cy = config.height;
	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mSupportVSync = FALSE;
	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(NULL, 0, CreateRenderThread, (IRenderThread*)this, NULL, &mRenderThreadId);

	if ((hr = mDDrawObj->WaitForVerticalBlank(DDWAITVB_BLOCKBEGINEVENT, mRenderEvent)) == DD_OK){
		mSupportVSync = TRUE;
	}

done:
	logger(Error, "%s", DXGetErrorStringA(hr));
	return hr != DD_OK;
}

BOOL DDrawRender::DeinitRender()
{
	mRenderThreadRuning = FALSE;
	if (mRenderThreadHandle != INVALID_HANDLE_VALUE){
		SetEvent(mRenderEvent);
		WaitForSingleObject(mRenderThreadHandle, INFINITE);
		mRenderThreadHandle = INVALID_HANDLE_VALUE;
	}

	if (mRenderEvent){
		CloseHandle(mRenderEvent);
		mRenderEvent = nullptr;
		mSupportVSync = FALSE;
	}

	SAFE_RELEASE(mCanvasSurface);
	SAFE_RELEASE(mPrimarySurface);
	SAFE_RELEASE(mDDrawClippper);
	SAFE_RELEASE(mDDrawObj);

	CoUninitialize();

	return TRUE;
}


DWORD DDrawRender::RenderLoop()
{
	HRESULT hr = S_FALSE;
	RECT rect = { 0 };
	DWORD renderBefore = 0;
	HDC dc = nullptr;
	uint32_t renderInterval = 0;

	mLastTime = 0;

	while (mRenderThreadRuning){
		int32_t minInputSample = 0, maxInputSample = 0, minRenderSample = 0, maxRenderSample = 0;
		if (mRenderEvent && mSupportVSync){
			if (WAIT_OBJECT_0 != WaitForSingleObject(mRenderEvent, INFINITE)){
				Sleep(10);
				continue;
			}
		} else {
			Sleep(16);
		}

		renderBefore = timeGetTime();

		if (mLastTime){
			renderInterval = renderBefore - mLastTime;
			mRenderStatis.AppendSample(renderInterval);
		}

		GetWindowRect(mHwnd, &rect);
		CHECK_HR(hr = mPrimarySurface->Blt(&rect, mCanvasSurface, nullptr, DDBLT_WAIT, nullptr));
		if (mInputStatis.SampleSize() > 2){
			mInputStatis.MinMaxSample(minInputSample, maxInputSample);
			mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);
		}

		CHECK_HR(hr = mPrimarySurface->GetDC(&dc));
		OSDText(dc, "fps %.2f, a:%llu(%2d~%2d), rd:%2u %2llu(%2d~%2d) %.2f",
			mInputStatis.Frequency(), mInputStatis.AvgSampleSize(), minInputSample, maxInputSample,
			renderInterval, mRenderStatis.AvgSampleSize(), minRenderSample, maxRenderSample, 1000.0/mRenderStatis.AvgSampleSize());
		mPrimarySurface->ReleaseDC(dc);

		mLastTime = renderBefore;

	done:
		continue;
	}

	return 0;
}

BOOL DDrawRender::PushFrame(VideoSampleBuffer *frame)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC2 desc;
	uint32_t ptsInterval = 0;
	int64_t ptss = 0, ptse = 0;
	int32_t *lineSize = 0;

	if (!frame){
		return FALSE;
	}


	lineSize = frame->planarStride;

	ptsInterval = (uint32_t)(frame->ptsStart - mLastPts);
	if (mLastPts)
		mInputStatis.AppendSample(ptsInterval);

	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	
	if (!mCanvasSurface){
		return S_FALSE;
	}
	CHECK_HR(hr = mCanvasSurface->Lock(nullptr, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr));

	if (!((uint32_t)frame->width > desc.dwWidth || (uint32_t)frame->height > desc.dwHeight)){
		uint8_t *surfaceBuffer = (uint8_t*)desc.lpSurface;
		if (frame->pixelFormatInFourCC == PIXEL_FORMAT_RGB24){
			for (int i = 0; i < frame->height; i++){
				DWORD *rgb32Buffer = rgb32Buffer = (DWORD*)(surfaceBuffer + i*desc.lPitch);
				uint8_t* rgb24Buffer = frame->GetDataPtr() + lineSize[0]*(frame->height - i);
				for (int j = 0; j < frame->width; j++){
					rgb32Buffer[j] = RGB(rgb24Buffer[0], rgb24Buffer[1], rgb24Buffer[2]);
					rgb24Buffer += 3;
				}
			}
		}
		else{
			memcpy(surfaceBuffer, frame->planarPtr[0], frame->validDataSize);
		}
	}

	mCanvasSurface->Unlock(nullptr);

	mLastPts = ptss;
	
done:
	if (FAILED(hr))
		logger(Error, "%s", DXGetErrorStringA(hr));

	return hr != DD_OK;
}

BOOL DDrawRender::OSDText(HDC hdc, char* format, ...)
{
	char buff[2048] = { 0 };
	va_list vl;
	va_start(vl, format);
	vsnprintf_s(buff, 2048, format, vl);
	va_end(vl);

	RECT rect = { 0 };
	GetWindowRect(mHwnd, &rect);

	int mode = SetBkMode(hdc, OPAQUE);
	TextOutA(hdc, rect.left, rect.bottom - 26, buff, strlen(buff));
	SetBkMode(hdc, mode);
	return TRUE;
}
