#include "stdafx.h"
#include "ddrawRender.h"

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
	, mWidth(0)
	, mHeight(0)
{
}

DDrawRender::DDrawRender(HWND hWnd)
	: mDDrawObj(NULL)
	, mDDrawPrimarySurface(NULL)
	, mDDrawSecondarySurface(NULL)
	, mHwnd(hWnd)
	, mWidth(0)
	, mHeight(0)
{
}

DDrawRender::~DDrawRender()
{
}

HRESULT DDrawRender::CreateSurfaces(int width, int height)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC desc;
	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawPrimarySurface, NULL));

	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = width;
	desc.dwHeight = height;
 	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	CHECK_HR(hr = mDDrawObj->CreateSurface(&desc, &mDDrawSecondarySurface, NULL));

done:
	GetDDrawErrorString(hr);
	return hr;
}

HRESULT DDrawRender::InitDDrawInterface(int width, int heigth)
{
	HRESULT hr = DD_OK;
	CHECK_HR(hr = CoInitialize(NULL));

	CHECK_HR(hr = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_ALL, IID_IDirectDraw, (void**)&mDDrawObj));
	CHECK_HR(hr = mDDrawObj->Initialize(NULL));
	CHECK_HR(hr = mDDrawObj->SetCooperativeLevel(mHwnd, DDSCL_NORMAL));

	CHECK_HR(hr = CreateSurfaces(width, heigth));

	DDPIXELFORMAT pixelFormat;
	ZeroMemory(&pixelFormat, sizeof(DDPIXELFORMAT));
	pixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	mDDrawPrimarySurface->GetPixelFormat(&pixelFormat);

	if (pixelFormat.dwFlags & DDPF_RGB){
		switch (pixelFormat.dwRGBBitCount){
			case 15: // must be 5.5.5 mode
			break;

			case 16: // must be 5.6.5 mode
			break;

			case 24: // must be 8.8.8 mode
			break;

			case 32: // must be alpha(8).8.8.8 mode
			break;

			default: break;
		} // end switch
	} // end if
	else{
		if (pixelFormat.dwFlags & DDPF_PALETTEINDEXED8){
			// 256 color palettized mode
		}else{
			// something else??? more tests
		} // end else
	}

	mWidth = width;
	mHeight = heigth;

done:
	GetDDrawErrorString(hr);
	return hr;
}

HRESULT DDrawRender::DeinitDDrawInterface()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(mDDrawSecondarySurface);
	SAFE_RELEASE(mDDrawPrimarySurface);
	SAFE_RELEASE(mDDrawObj);

	CoUninitialize();

	return hr;
}

HRESULT DDrawRender::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = DD_OK;
	DDSURFACEDESC desc;
	ZeroMemory(&desc, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	if (FAILED(mDDrawPrimarySurface->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))){
		// 
		printf("error");
	}

 	uint8_t *buffer = (uint8_t*)desc.lpSurface;
	uint8_t *bufferLine = NULL;

	uint8_t *frameBuffer = frame->GetDataPtr();
	uint8_t *frameLine = NULL;
	int line_size = desc.lPitch;
	int line_size_frame = frame->GetLineSize();

	for (DWORD i = 0; i < desc.dwHeight; i++){
		bufferLine = (uint8_t*)(buffer + line_size*i);
		frameLine = frameBuffer + line_size_frame*i;
		for (DWORD j = 0; j < desc.dwWidth; j++){
			//bufferLine[i] = frameBuffer[j];
			memcpy(bufferLine, frameLine, min(line_size, line_size_frame));
		}
	}

	//memcpy(buffer, frame->GetDataPtr(), frame->GetDataSize());

	mDDrawPrimarySurface->Unlock(NULL);
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
