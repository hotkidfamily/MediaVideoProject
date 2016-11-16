#include "stdafx.h"
#include "ddrawRender.h"
#include <DShow.h>

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
	, mDDrawPrimarySurface(NULL)
	, mDDrawSecondarySurface(NULL)
	, mHwnd(NULL)
	, mRenderThreadHandle(NULL)
	, mRenderThreadId(0)
	, mRenderEvent(NULL)
{
}

DDrawRender::DDrawRender(HWND hWnd)
	: mDDrawObj(NULL)
	, mDDrawPrimarySurface(NULL)
	, mDDrawSecondarySurface(NULL)
	, mHwnd(hWnd)
	, mRenderThreadHandle(NULL)
	, mRenderThreadId(0)
	, mRenderEvent(NULL)
{
}

DDrawRender::~DDrawRender()
{
}

HRESULT DDrawRender::CreateSurfaces(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawPrimarySurface, NULL));

	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	desc.dwWidth = width;
	desc.dwHeight = height;
 	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	desc.ddpfPixelFormat.dwFourCC = pixelFormatInFourCC;

	if (pixelFormatInFourCC == PIXEL_FORMAT_RGB24){
		desc.ddpfPixelFormat.dwFourCC = BI_RGB;
		desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
		desc.ddpfPixelFormat.dwRGBBitCount = 24;
		desc.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		desc.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		desc.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0;
	}else{
		desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
		desc.ddpfPixelFormat.dwYUVBitCount = 8;
	}

	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawSecondarySurface, NULL));

done:
	GetDDrawErrorString(hr);
	return hr;
}

DWORD WINAPI RenderThread(LPVOID args)
{
	DDrawRender *pRender = (DDrawRender*)args;

	return TRUE;
}

DWORD DDrawRender::RenderLoop()
{
	while (bRender){
		
	}

	return 0;
}

HRESULT DDrawRender::InitDDrawInterface(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = DD_OK;
	CHECK_HR(hr = CoInitialize(NULL));

	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_ALL, IID_IDirectDraw, (void**)&mDDrawObj));
	CHECK_HR(hr = mDDrawObj->Initialize(NULL));
	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));

	CHECK_HR(hr = CreateSurfaces(width, height, pixelFormatInFourCC));

	CHECK_HR(hr = mDDrawObj->CreateClipper(0, &mDDrawClippper, NULL));
	CHECK_HR(hr = mDDrawClippper->SetHWnd(0, mHwnd));
	CHECK_HR(hr = mDDrawPrimarySurface->SetClipper(mDDrawClippper));

	mLastPts = 0;

	mRenderThreadHandle = CreateThread(NULL, 0, RenderThread, this, NULL, &mRenderThreadId);

done:
	GetDDrawErrorString(hr);
	return hr;
}

HRESULT DDrawRender::DeinitDDrawInterface()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(mDDrawSecondarySurface);
	SAFE_RELEASE(mDDrawPrimarySurface);
	SAFE_RELEASE(mDDrawClippper);
	SAFE_RELEASE(mDDrawObj);

	CoUninitialize();


	return hr;
}

HRESULT DDrawRender::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	RECT rect;
	GetWindowRect(mHwnd,&rect);

	if (FAILED(hr = mDDrawSecondarySurface->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))){
		goto done;
	}

	if (frame->GetWidth() > desc.dwWidth || frame->GetHeight() > desc.dwHeight){

	}else{
		uint8_t *surfaceBuffer = (uint8_t*)desc.lpSurface;
		memcpy(surfaceBuffer, frame->GetDataPtr(), frame->GetDataSize());
	}

	mDDrawSecondarySurface->Unlock(NULL);

	HDC dc;
	hr = mDDrawPrimarySurface->GetDC(&dc);
#define TEXTCHAR "hello world"
	TextOutA(dc, rect.left, rect.top + 100, TEXTCHAR, strlen(TEXTCHAR));
	mDDrawPrimarySurface->ReleaseDC(dc);

	DDBLTFX ddblfx;
	ZeroMemory(&ddblfx, sizeof(DDBLTFX));
	ddblfx.dwSize = sizeof(DDBLTFX);
	ddblfx.dwROP = SRCCOPY;

	mDDrawPrimarySurface->Blt(&rect, mDDrawSecondarySurface, NULL, DDBLT_ROP, &ddblfx);
	mLastPts = frame->GetPts();

done:
	GetDDrawErrorString(hr);
	return hr;
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
