#include "stdafx.h"
#include "ddrawRender.h"
#include <stdlib.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

const COMERROR ddrawErrorList[] = {
	COMERROR2STR(DD_OK),
	COMERROR2STR(DDERR_ALREADYINITIALIZED),
	COMERROR2STR(DDERR_CANNOTATTACHSURFACE),
	COMERROR2STR(DDERR_CANNOTDETACHSURFACE),
	COMERROR2STR(DDERR_CURRENTLYNOTAVAIL),
	COMERROR2STR(DDERR_EXCEPTION),
	COMERROR2STR(DDERR_GENERIC),
	COMERROR2STR(DDERR_HEIGHTALIGN),
	COMERROR2STR(DDERR_INCOMPATIBLEPRIMARY),
	COMERROR2STR(DDERR_INVALIDCAPS),
	COMERROR2STR(DDERR_INVALIDCLIPLIST),
	COMERROR2STR(DDERR_INVALIDMODE),
	COMERROR2STR(DDERR_INVALIDOBJECT),
	COMERROR2STR(DDERR_INVALIDPARAMS),
	COMERROR2STR(DDERR_INVALIDPIXELFORMAT),
	COMERROR2STR(DDERR_INVALIDRECT),
	COMERROR2STR(DDERR_LOCKEDSURFACES),
	COMERROR2STR(DDERR_NO3D),
	COMERROR2STR(DDERR_NOALPHAHW),
	COMERROR2STR(DDERR_NOSTEREOHARDWARE),
	COMERROR2STR(DDERR_NOSURFACELEFT),
	COMERROR2STR(DDERR_NOCLIPLIST),
	COMERROR2STR(DDERR_NOCOLORCONVHW),
	COMERROR2STR(DDERR_NOCOOPERATIVELEVELSET),
	COMERROR2STR(DDERR_NOCOLORKEY),
	COMERROR2STR(DDERR_NOCOLORKEYHW),
	COMERROR2STR(DDERR_NODIRECTDRAWSUPPORT),
	COMERROR2STR(DDERR_NOEXCLUSIVEMODE),
	COMERROR2STR(DDERR_NOFLIPHW),
	COMERROR2STR(DDERR_NOGDI),
	COMERROR2STR(DDERR_NOMIRRORHW),
	COMERROR2STR(DDERR_NOTFOUND),
	COMERROR2STR(DDERR_NOOVERLAYHW),
	COMERROR2STR(DDERR_OVERLAPPINGRECTS),
	COMERROR2STR(DDERR_NORASTEROPHW),
	COMERROR2STR(DDERR_NOROTATIONHW),
	COMERROR2STR(DDERR_NOSTRETCHHW),
	COMERROR2STR(DDERR_NOT4BITCOLOR),
	COMERROR2STR(DDERR_NOT4BITCOLORINDEX),
	COMERROR2STR(DDERR_NOT8BITCOLOR),
	COMERROR2STR(DDERR_NOTEXTUREHW),
	COMERROR2STR(DDERR_NOVSYNCHW),
	COMERROR2STR(DDERR_NOZBUFFERHW),
	COMERROR2STR(DDERR_NOZOVERLAYHW),
	COMERROR2STR(DDERR_OUTOFCAPS),
	COMERROR2STR(DDERR_OUTOFMEMORY),
	COMERROR2STR(DDERR_OUTOFVIDEOMEMORY),
	COMERROR2STR(DDERR_OVERLAYCANTCLIP),
	COMERROR2STR(DDERR_OVERLAYCOLORKEYONLYONEACTIVE),
	COMERROR2STR(DDERR_PALETTEBUSY),
	COMERROR2STR(DDERR_COLORKEYNOTSET),
	COMERROR2STR(DDERR_SURFACEALREADYATTACHED),
	COMERROR2STR(DDERR_SURFACEALREADYDEPENDENT),
	COMERROR2STR(DDERR_SURFACEBUSY),
	COMERROR2STR(DDERR_CANTLOCKSURFACE),
	COMERROR2STR(DDERR_SURFACEISOBSCURED),
	COMERROR2STR(DDERR_SURFACELOST),
	COMERROR2STR(DDERR_SURFACENOTATTACHED),
	COMERROR2STR(DDERR_TOOBIGHEIGHT),
	COMERROR2STR(DDERR_TOOBIGSIZE),
	COMERROR2STR(DDERR_TOOBIGWIDTH),
	COMERROR2STR(DDERR_UNSUPPORTED),
	COMERROR2STR(DDERR_UNSUPPORTEDFORMAT),
	COMERROR2STR(DDERR_UNSUPPORTEDMASK),
	COMERROR2STR(DDERR_INVALIDSTREAM),
	COMERROR2STR(DDERR_VERTICALBLANKINPROGRESS),
	COMERROR2STR(DDERR_WASSTILLDRAWING),
	COMERROR2STR(DDERR_DDSCAPSCOMPLEXREQUIRED),
	COMERROR2STR(DDERR_XALIGN),
	COMERROR2STR(DDERR_INVALIDDIRECTDRAWGUID),
	COMERROR2STR(DDERR_DIRECTDRAWALREADYCREATED),
	COMERROR2STR(DDERR_NODIRECTDRAWHW),
	COMERROR2STR(DDERR_PRIMARYSURFACEALREADYEXISTS),
	COMERROR2STR(DDERR_NOEMULATION),
	COMERROR2STR(DDERR_REGIONTOOSMALL),
	COMERROR2STR(DDERR_CLIPPERISUSINGHWND),
	COMERROR2STR(DDERR_NOCLIPPERATTACHED),
	COMERROR2STR(DDERR_NOHWND),
	COMERROR2STR(DDERR_HWNDSUBCLASSED),
	COMERROR2STR(DDERR_HWNDALREADYSET),
	COMERROR2STR(DDERR_NOPALETTEATTACHED),
	COMERROR2STR(DDERR_NOPALETTEHW),
	COMERROR2STR(DDERR_BLTFASTCANTCLIP),
	COMERROR2STR(DDERR_NOBLTHW),
	COMERROR2STR(DDERR_NODDROPSHW),
	COMERROR2STR(DDERR_OVERLAYNOTVISIBLE),
	COMERROR2STR(DDERR_NOOVERLAYDEST),
	COMERROR2STR(DDERR_INVALIDPOSITION),
	COMERROR2STR(DDERR_NOTAOVERLAYSURFACE),
	COMERROR2STR(DDERR_EXCLUSIVEMODEALREADYSET),
	COMERROR2STR(DDERR_NOTFLIPPABLE),
	COMERROR2STR(DDERR_CANTDUPLICATE),
	COMERROR2STR(DDERR_NOTLOCKED),
	COMERROR2STR(DDERR_CANTCREATEDC),
	COMERROR2STR(DDERR_NODC),
	COMERROR2STR(DDERR_WRONGMODE),
	COMERROR2STR(DDERR_IMPLICITLYCREATED),
	COMERROR2STR(DDERR_NOTPALETTIZED),
	COMERROR2STR(DDERR_UNSUPPORTEDMODE),
	COMERROR2STR(DDERR_NOMIPMAPHW),
	COMERROR2STR(DDERR_INVALIDSURFACETYPE),
	COMERROR2STR(DDERR_NOOPTIMIZEHW),
	COMERROR2STR(DDERR_NOTLOADED),
	COMERROR2STR(DDERR_NOFOCUSWINDOW),
	COMERROR2STR(DDERR_NOTONMIPMAPSUBLEVEL),
	COMERROR2STR(DDERR_DCALREADYCREATED),
	COMERROR2STR(DDERR_NONONLOCALVIDMEM),
	COMERROR2STR(DDERR_CANTPAGELOCK),
	COMERROR2STR(DDERR_CANTPAGEUNLOCK),
	COMERROR2STR(DDERR_NOTPAGELOCKED),
	COMERROR2STR(DDERR_MOREDATA),
	COMERROR2STR(DDERR_EXPIRED),
	COMERROR2STR(DDERR_TESTFINISHED),
	COMERROR2STR(DDERR_NEWMODE),
	COMERROR2STR(DDERR_D3DNOTINITIALIZED),
	COMERROR2STR(DDERR_VIDEONOTACTIVE),
	COMERROR2STR(DDERR_NOMONITORINFORMATION),
	COMERROR2STR(DDERR_NODRIVERSUPPORT),
	COMERROR2STR(DDERR_DEVICEDOESNTOWNSURFACE),
	COMERROR2STR(DDERR_NOTINITIALIZED)
};

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

DDrawRender::DDrawRender(HWND hWnd)
	: mDDrawObj(nullptr)
	, mPrimarySurface(nullptr)
	, mCanvasSurface(nullptr)
	, mHwnd(hWnd)
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
	GetErrorString(hr);
	return hr;
}

DWORD WINAPI RenderThread(LPVOID args)
{
	DDrawRender *pRender = (DDrawRender*)args;
	return pRender->RenderLoop();
}

BOOL DDrawRender::InitRender(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = DD_OK;
	DDBLTFX ddbltfx = { 0 };
	ZeroMemory(&mHwCaps, sizeof(DDCAPS));
	ZeroMemory(&mHelCaps, sizeof(DDCAPS));

	CHECK_HR(hr = CoInitialize(nullptr));

	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, nullptr, CLSCTX_ALL, IID_IDirectDraw, (void**)&mDDrawObj));
	CHECK_HR(hr = mDDrawObj->Initialize(nullptr));
	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));

	mHwCaps.dwSize = sizeof(DDCAPS);
	mHelCaps.dwSize = sizeof(DDCAPS);
	mDDrawObj->GetCaps(&mHwCaps, &mHelCaps);

	CHECK_HR(hr = CreateSurfaces(width, height, pixelFormatInFourCC));

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
	mScreenSizeInPixel.cx = width;
	mScreenSizeInPixel.cy = height;
	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mSupportVSync = FALSE;
	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(NULL, 0, RenderThread, this, NULL, &mRenderThreadId);

	if ((hr = mDDrawObj->WaitForVerticalBlank(DDWAITVB_BLOCKBEGINEVENT, mRenderEvent)) == DD_OK){
		mSupportVSync = TRUE;
	}

done:
	GetErrorString(hr);
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

BOOL DDrawRender::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC2 desc;
	uint32_t ptsInterval = (uint32_t)(frame->GetPts() - mLastPts);
	if (mLastPts)
		mInputStatis.AppendSample(ptsInterval);

	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	
	if (!mCanvasSurface){
		return S_FALSE;
	}
	CHECK_HR(hr = mCanvasSurface->Lock(nullptr, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr));

	if (!((uint32_t)frame->GetWidth() > desc.dwWidth || (uint32_t)frame->GetHeight() > desc.dwHeight)){
		uint8_t *surfaceBuffer = (uint8_t*)desc.lpSurface;
		if (frame->GetPixelFormat() == PIXEL_FORMAT_RGB24){
			for (int i = 0; i < frame->GetHeight(); i++){
				DWORD *rgb32Buffer = rgb32Buffer = (DWORD*)(surfaceBuffer + i*desc.lPitch);
				uint8_t* rgb24Buffer = frame->GetDataPtr() + frame->GetLineSize()*(frame->GetHeight()-i);
				for (int j = 0; j < frame->GetWidth(); j++){
					rgb32Buffer[j] = RGB(rgb24Buffer[0], rgb24Buffer[1], rgb24Buffer[2]);
					rgb24Buffer += 3;
				}
			}
		}
		else{
			memcpy(surfaceBuffer, frame->GetDataPtr(), frame->GetDataSize());
		}
	}

	mCanvasSurface->Unlock(nullptr);

	mLastPts = frame->GetPts();
	
done:
	GetErrorString(hr);
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

const TCHAR* DDrawRender::GetErrorString(HRESULT hr)
{
	const COMERROR *prr = &ddrawErrorList[DD_OK];

	if (FAILED(hr)){
		for (int i = 0; i < ARRAYSIZE(ddrawErrorList); i++){
			if (ddrawErrorList[i].hr == hr){
				prr = &ddrawErrorList[i];
			}
		}
	}

	return prr->desc;
}
