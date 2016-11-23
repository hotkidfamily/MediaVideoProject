#include "stdafx.h"
#include "ddrawRender.h"
#include <stdlib.h>

#define to_pair(x) {x, #x}
typedef struct tagddrawError{
	HRESULT hr;
	const char* desc;
}DDRAWERROR;
DDRAWERROR errorList[] = {
	to_pair(DD_OK),
	to_pair(DDERR_ALREADYINITIALIZED),
	to_pair(DDERR_CANNOTATTACHSURFACE),
	to_pair(DDERR_CANNOTDETACHSURFACE),
	to_pair(DDERR_CURRENTLYNOTAVAIL),
	to_pair(DDERR_EXCEPTION),
	to_pair(DDERR_GENERIC),
	to_pair(DDERR_HEIGHTALIGN),
	to_pair(DDERR_INCOMPATIBLEPRIMARY),
	to_pair(DDERR_INVALIDCAPS),
	to_pair(DDERR_INVALIDCLIPLIST),
	to_pair(DDERR_INVALIDMODE),
	to_pair(DDERR_INVALIDOBJECT),
	to_pair(DDERR_INVALIDPARAMS),
	to_pair(DDERR_INVALIDPIXELFORMAT),
	to_pair(DDERR_INVALIDRECT),
	to_pair(DDERR_LOCKEDSURFACES),
	to_pair(DDERR_NO3D),
	to_pair(DDERR_NOALPHAHW),
	to_pair(DDERR_NOSTEREOHARDWARE),
	to_pair(DDERR_NOSURFACELEFT),
	to_pair(DDERR_NOCLIPLIST),
	to_pair(DDERR_NOCOLORCONVHW),
	to_pair(DDERR_NOCOOPERATIVELEVELSET),
	to_pair(DDERR_NOCOLORKEY),
	to_pair(DDERR_NOCOLORKEYHW),
	to_pair(DDERR_NODIRECTDRAWSUPPORT),
	to_pair(DDERR_NOEXCLUSIVEMODE),
	to_pair(DDERR_NOFLIPHW),
	to_pair(DDERR_NOGDI),
	to_pair(DDERR_NOMIRRORHW),
	to_pair(DDERR_NOTFOUND),
	to_pair(DDERR_NOOVERLAYHW),
	to_pair(DDERR_OVERLAPPINGRECTS),
	to_pair(DDERR_NORASTEROPHW),
	to_pair(DDERR_NOROTATIONHW),
	to_pair(DDERR_NOSTRETCHHW),
	to_pair(DDERR_NOT4BITCOLOR),
	to_pair(DDERR_NOT4BITCOLORINDEX),
	to_pair(DDERR_NOT8BITCOLOR),
	to_pair(DDERR_NOTEXTUREHW),
	to_pair(DDERR_NOVSYNCHW),
	to_pair(DDERR_NOZBUFFERHW),
	to_pair(DDERR_NOZOVERLAYHW),
	to_pair(DDERR_OUTOFCAPS),
	to_pair(DDERR_OUTOFMEMORY),
	to_pair(DDERR_OUTOFVIDEOMEMORY),
	to_pair(DDERR_OVERLAYCANTCLIP),
	to_pair(DDERR_OVERLAYCOLORKEYONLYONEACTIVE),
	to_pair(DDERR_PALETTEBUSY),
	to_pair(DDERR_COLORKEYNOTSET),
	to_pair(DDERR_SURFACEALREADYATTACHED),
	to_pair(DDERR_SURFACEALREADYDEPENDENT),
	to_pair(DDERR_SURFACEBUSY),
	to_pair(DDERR_CANTLOCKSURFACE),
	to_pair(DDERR_SURFACEISOBSCURED),
	to_pair(DDERR_SURFACELOST),
	to_pair(DDERR_SURFACENOTATTACHED),
	to_pair(DDERR_TOOBIGHEIGHT),
	to_pair(DDERR_TOOBIGSIZE),
	to_pair(DDERR_TOOBIGWIDTH),
	to_pair(DDERR_UNSUPPORTED),
	to_pair(DDERR_UNSUPPORTEDFORMAT),
	to_pair(DDERR_UNSUPPORTEDMASK),
	to_pair(DDERR_INVALIDSTREAM),
	to_pair(DDERR_VERTICALBLANKINPROGRESS),
	to_pair(DDERR_WASSTILLDRAWING),
	to_pair(DDERR_DDSCAPSCOMPLEXREQUIRED),
	to_pair(DDERR_XALIGN),
	to_pair(DDERR_INVALIDDIRECTDRAWGUID),
	to_pair(DDERR_DIRECTDRAWALREADYCREATED),
	to_pair(DDERR_NODIRECTDRAWHW),
	to_pair(DDERR_PRIMARYSURFACEALREADYEXISTS),
	to_pair(DDERR_NOEMULATION),
	to_pair(DDERR_REGIONTOOSMALL),
	to_pair(DDERR_CLIPPERISUSINGHWND),
	to_pair(DDERR_NOCLIPPERATTACHED),
	to_pair(DDERR_NOHWND),
	to_pair(DDERR_HWNDSUBCLASSED),
	to_pair(DDERR_HWNDALREADYSET),
	to_pair(DDERR_NOPALETTEATTACHED),
	to_pair(DDERR_NOPALETTEHW),
	to_pair(DDERR_BLTFASTCANTCLIP),
	to_pair(DDERR_NOBLTHW),
	to_pair(DDERR_NODDROPSHW),
	to_pair(DDERR_OVERLAYNOTVISIBLE),
	to_pair(DDERR_NOOVERLAYDEST),
	to_pair(DDERR_INVALIDPOSITION),
	to_pair(DDERR_NOTAOVERLAYSURFACE),
	to_pair(DDERR_EXCLUSIVEMODEALREADYSET),
	to_pair(DDERR_NOTFLIPPABLE),
	to_pair(DDERR_CANTDUPLICATE),
	to_pair(DDERR_NOTLOCKED),
	to_pair(DDERR_CANTCREATEDC),
	to_pair(DDERR_NODC),
	to_pair(DDERR_WRONGMODE),
	to_pair(DDERR_IMPLICITLYCREATED),
	to_pair(DDERR_NOTPALETTIZED),
	to_pair(DDERR_UNSUPPORTEDMODE),
	to_pair(DDERR_NOMIPMAPHW),
	to_pair(DDERR_INVALIDSURFACETYPE),
	to_pair(DDERR_NOOPTIMIZEHW),
	to_pair(DDERR_NOTLOADED),
	to_pair(DDERR_NOFOCUSWINDOW),
	to_pair(DDERR_NOTONMIPMAPSUBLEVEL),
	to_pair(DDERR_DCALREADYCREATED),
	to_pair(DDERR_NONONLOCALVIDMEM),
	to_pair(DDERR_CANTPAGELOCK),
	to_pair(DDERR_CANTPAGEUNLOCK),
	to_pair(DDERR_NOTPAGELOCKED),
	to_pair(DDERR_MOREDATA),
	to_pair(DDERR_EXPIRED),
	to_pair(DDERR_TESTFINISHED),
	to_pair(DDERR_NEWMODE),
	to_pair(DDERR_D3DNOTINITIALIZED),
	to_pair(DDERR_VIDEONOTACTIVE),
	to_pair(DDERR_NOMONITORINFORMATION),
	to_pair(DDERR_NODRIVERSUPPORT),
	to_pair(DDERR_DEVICEDOESNTOWNSURFACE),
	to_pair(DDERR_NOTINITIALIZED)
};
#undef to_pair

DDrawRender::DDrawRender()
	: mDDrawObj(NULL)
	, mPrimarySurface(NULL)
	, mCanvasSurface(NULL)
	, mHwnd(NULL)
	, mRenderThreadHandle(NULL)
	, mRenderThreadId(0)
	, mRenderEvent(NULL)
	, mDDrawClippper(NULL)
{
}

DDrawRender::DDrawRender(HWND hWnd)
	: mDDrawObj(NULL)
	, mPrimarySurface(NULL)
	, mCanvasSurface(NULL)
	, mHwnd(hWnd)
	, mRenderThreadHandle(NULL)
	, mRenderThreadId(0)
	, mRenderEvent(NULL)
	, mDDrawClippper(NULL)
{
}

DDrawRender::~DDrawRender()
{
	DeinitDDrawInterface();
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

	CHECK_HR(hr = mDDrawObj->CreateSurface(&ddsd, &mPrimarySurface, NULL));

	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	FillddPixelFormatFromFourCC(&(ddsd.ddpfPixelFormat), pixelFormatInFourCC);

	CHECK_HR(hr = mDDrawObj->CreateSurface(&ddsd, &mCanvasSurface, NULL));

	mCanvasBpp = ddsd.ddpfPixelFormat.dwRGBBitCount;

done:
	GetDDrawErrorString(hr);
	return hr;
}

DWORD WINAPI RenderThread(LPVOID args)
{
	DDrawRender *pRender = (DDrawRender*)args;
	return pRender->RenderLoop();
}

HRESULT DDrawRender::InitDDrawInterface(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = DD_OK;
	DDBLTFX ddbltfx = { 0 };
	ZeroMemory(&mHwCaps, sizeof(DDCAPS));
	ZeroMemory(&mHelCaps, sizeof(DDCAPS));

	CHECK_HR(hr = CoInitialize(NULL));

	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_ALL, IID_IDirectDraw, (void**)&mDDrawObj));
	CHECK_HR(hr = mDDrawObj->Initialize(NULL));
	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));
	
	mHwCaps.dwSize = sizeof(DDCAPS);
	mHelCaps.dwSize = sizeof(DDCAPS);
	mDDrawObj->GetCaps(&mHwCaps, &mHelCaps);
	
	CHECK_HR(hr = CreateSurfaces(width, height, pixelFormatInFourCC));

	CHECK_HR(hr = mDDrawObj->CreateClipper(0, &mDDrawClippper, NULL));
	CHECK_HR(hr = mDDrawClippper->SetHWnd(0, mHwnd));
	CHECK_HR(hr = mPrimarySurface->SetClipper(mDDrawClippper));
	
	/* clear screen */
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = RGB(0, 0, 0);
	hr = mPrimarySurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	if (hr == DDERR_SURFACELOST){
		//Restore surface
	}

	mLastPts = 0;
	mLastTime = 0;
	mScreenSizeInPixel.cx = width;
	mScreenSizeInPixel.cy = height;

	mRenderThreadHandle = CreateThread(NULL, 0, RenderThread, this, NULL, &mRenderThreadId);

done:
	GetDDrawErrorString(hr);
	return hr;
}

HRESULT DDrawRender::DeinitDDrawInterface()
{
	HRESULT hr = S_OK;

	bRender = FALSE;
	if (mRenderThreadHandle)
		WaitForSingleObject(mRenderThreadHandle, INFINITE);

	SAFE_RELEASE(mCanvasSurface);
	SAFE_RELEASE(mPrimarySurface);
	SAFE_RELEASE(mDDrawClippper);
	SAFE_RELEASE(mDDrawObj);

	CoUninitialize();
	return hr;
}


DWORD DDrawRender::RenderLoop()
{
	HRESULT hr = S_FALSE;
	RECT rect = { 0 };
	DWORD renderBefore = 0;
	HDC dc = NULL;
	uint32_t renderInterval = 0;

	mLastTime = 0;

	while (bRender){
		int32_t minInputSample = 0, maxInputSample = 0, minRenderSample = 0, maxRenderSample = 0;
		//Sleep(rand() % 15 + 20);
		Sleep(15);
		renderBefore = timeGetTime();

		if (mLastTime){
			renderInterval = renderBefore - mLastTime;
			mRenderStatis.AppendSample(renderInterval);
		}

		GetWindowRect(mHwnd, &rect);
		CHECK_HR(hr = mPrimarySurface->Blt(&rect, mCanvasSurface, NULL, DDBLT_WAIT, NULL));
		if (mInputStatis.SampleSize() > 2){
			mInputStatis.MinMaxSample(minInputSample, maxInputSample);
			mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);
		}

		CHECK_HR(hr = mPrimarySurface->GetDC(&dc));
		OSDText(dc, "fps %.2f, a:%lld(%2d~%2d), rd:%2u %2lld(%2d~%2d) %.2f",
			mInputStatis.Frequency(), mInputStatis.AvgSampleSize(), minInputSample, maxInputSample,
			renderInterval, mRenderStatis.AvgSampleSize(), minRenderSample, maxRenderSample, 1000.0/mRenderStatis.AvgSampleSize());
		mPrimarySurface->ReleaseDC(dc);

		mLastTime = renderBefore;

	done:
		continue;
	}

	return 0;
}

HRESULT DDrawRender::PushFrame(CSampleBuffer *frame)
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
	CHECK_HR(hr = mCanvasSurface->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL));

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

	mCanvasSurface->Unlock(NULL);

	mLastPts = frame->GetPts();
	
done:
	GetDDrawErrorString(hr);
	return hr;
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

const char* DDrawRender::GetDDrawErrorString(HRESULT hr)
{
	DDRAWERROR *prr = &errorList[DD_OK];

	if (FAILED(hr)){
		for (int i = 0; i < ARRAYSIZE(errorList); i++){
			if (errorList[i].hr == hr){
				prr = &errorList[i];
			}
		}
	}

	return prr->desc;
}
