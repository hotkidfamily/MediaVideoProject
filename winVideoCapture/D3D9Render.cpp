#include "stdafx.h"
#include "D3D9Render.h"


const COMERROR d3d9ErrorList[] = {
	COMERROR2STR(D3D_OK),
	COMERROR2STR(D3DERR_WRONGTEXTUREFORMAT),
	COMERROR2STR(D3DERR_UNSUPPORTEDCOLOROPERATION),
	COMERROR2STR(D3DERR_UNSUPPORTEDCOLORARG),
	COMERROR2STR(D3DERR_UNSUPPORTEDALPHAOPERATION),
	COMERROR2STR(D3DERR_UNSUPPORTEDALPHAARG),
	COMERROR2STR(D3DERR_TOOMANYOPERATIONS),
	COMERROR2STR(D3DERR_CONFLICTINGTEXTUREFILTER),
	COMERROR2STR(D3DERR_UNSUPPORTEDFACTORVALUE),
	COMERROR2STR(D3DERR_CONFLICTINGRENDERSTATE),
	COMERROR2STR(D3DERR_UNSUPPORTEDTEXTUREFILTER),
	COMERROR2STR(D3DERR_CONFLICTINGTEXTUREPALETTE),
	COMERROR2STR(D3DERR_DRIVERINTERNALERROR),
	COMERROR2STR(D3DERR_NOTFOUND),
	COMERROR2STR(D3DERR_MOREDATA),
	COMERROR2STR(D3DERR_DEVICELOST),
	COMERROR2STR(D3DERR_DEVICENOTRESET),
	COMERROR2STR(D3DERR_NOTAVAILABLE),
	COMERROR2STR(D3DERR_OUTOFVIDEOMEMORY),
	COMERROR2STR(D3DERR_INVALIDDEVICE),
	COMERROR2STR(D3DERR_INVALIDCALL),
	COMERROR2STR(D3DERR_DRIVERINVALIDCALL),
	COMERROR2STR(D3DERR_WASSTILLDRAWING),
	COMERROR2STR(D3DOK_NOAUTOGEN),
	COMERROR2STR(D3DERR_DEVICEREMOVED),
	COMERROR2STR(S_NOT_RESIDENT),
	COMERROR2STR(S_RESIDENT_IN_SHARED_MEMORY),
	COMERROR2STR(S_PRESENT_MODE_CHANGED),
	COMERROR2STR(S_PRESENT_OCCLUDED),
	COMERROR2STR(D3DERR_DEVICEHUNG),
	COMERROR2STR(D3DERR_UNSUPPORTEDOVERLAY),
	COMERROR2STR(D3DERR_UNSUPPORTEDOVERLAYFORMAT),
	COMERROR2STR(D3DERR_CANNOTPROTECTCONTENT),
	COMERROR2STR(D3DERR_UNSUPPORTEDCRYPTO),
	COMERROR2STR(D3DERR_PRESENT_STATISTICS_DISJOINT)
};

D3D9Render::D3D9Render()
	: mhWnd(NULL)
	, mpD3D9OBj(NULL)
	, mpD3D9Device(NULL)
	, mPFont(NULL)
	, mPrimerySurface(NULL)

	, mRenderEvent(NULL)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
	, cs({0})
{
}

D3D9Render::D3D9Render(HWND hWnd)
	: mhWnd(hWnd)
	, mpD3D9OBj(NULL)
	, mpD3D9Device(NULL)
	, mPFont(NULL)
	, mPrimerySurface(NULL)

	, mRenderEvent(NULL)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
	, cs({ 0 })
{

}


D3D9Render::~D3D9Render()
{
}


DWORD WINAPI RenderThread(LPVOID args)
{
	D3D9Render *pRender = (D3D9Render*)args;
	return pRender->RenderLoop();
}

void D3D9Render::FourCCtoD3DFormat(D3DFORMAT* pd3dPixelFormat, DWORD dwFourCC)
{
	switch (dwFourCC)
	{
	case PIXEL_FORMAT_RGB24:
	case PIXEL_FORMAT_RGB32:
	case PIXEL_FORMAT_ARGB:
		*pd3dPixelFormat = D3DFMT_A8R8G8B8;
		break;
	case PIXEL_FORMAT_RGB565:
		*pd3dPixelFormat = D3DFMT_X1R5G5B5;
		break;
	case PIXEL_FORMAT_RGB555:
		*pd3dPixelFormat = D3DFMT_R5G6B5;
		break;
	case PIXEL_FORMAT_UYVY:
	case PIXEL_FORMAT_YUY2:
		*pd3dPixelFormat = (D3DFORMAT)dwFourCC;
		break;
	case PIXEL_FORMAT_YV12:
	default:
		*pd3dPixelFormat = D3DFMT_UNKNOWN;
		break;
	}
}

HRESULT D3D9Render::InitializeRenderContext(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = S_OK;
	RECT rect = { 0 };
	D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device

	InitializeCriticalSection(&cs);

	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = TRUE; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = mhWnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 1; //set it to only use 1 backbuffer
	d3dpp.BackBufferWidth = width; //set the buffer to our window width
	d3dpp.BackBufferHeight = height; //set the buffer to out window height
	FourCCtoD3DFormat(&d3dpp.BackBufferFormat, pixelFormatInFourCC);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //SwapEffect

	mpD3D9OBj = Direct3DCreate9(D3D_SDK_VERSION); //Create the presentation parameters

	if (FAILED(mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.BackBufferFormat, d3dpp.BackBufferFormat, TRUE))){
		// need vpp 
	}

	CHECK_HR(hr = mpD3D9OBj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mhWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mpD3D9Device));
	CHECK_HR(hr = D3DXCreateFont(mpD3D9Device, 30, 0, FW_LIGHT, 1, TRUE, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

	CHECK_HR(hr = mpD3D9Device->CreateOffscreenPlainSurface(width, height, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &mPrimerySurface, NULL));
	//CHECK_HR(hr = mPD3DDevice->CreateRenderTarget(width, height, d3dpp.BackBufferFormat, 0, 0, TRUE, &mPrimerySurface, NULL));
	CHECK_HR(hr = mpD3D9Device->GetDeviceCaps(&mpD3D9DeviceCaps));



	mRenderEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mSupportVSync = FALSE;
	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(NULL, 0, RenderThread, this, NULL, &mRenderThreadId);

done:
	if (FAILED(hr)){
		DeinitRenderContext();
	}

	GetErrorString(hr);

	return hr;
}

HRESULT D3D9Render::DeinitRenderContext()
{
	mRenderThreadRuning = FALSE;
	if (mRenderThreadHandle != INVALID_HANDLE_VALUE){
		SetEvent(mRenderEvent);
		WaitForSingleObject(mRenderThreadHandle, INFINITE);
		mRenderThreadHandle = INVALID_HANDLE_VALUE;
	}

	if (mRenderEvent){
		CloseHandle(mRenderEvent);
		mRenderEvent = NULL;
		mSupportVSync = FALSE;
	}

	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mpD3D9Device);
	SAFE_RELEASE(mpD3D9OBj);

	DeleteCriticalSection(&cs);

	return S_OK;
}

DWORD D3D9Render::RenderLoop()
{
	HRESULT hr = S_OK;

	while (mRenderThreadRuning){
		Sleep(10);
		IDirect3DSurface9 *pBackBuffer = NULL;
		CAutoLock lock(cs);

		if (SUCCEEDED(hr = mpD3D9Device->BeginScene())){
			CHECK_HR(hr = mpD3D9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer));
			CHECK_HR(hr = mpD3D9Device->UpdateSurface(mPrimerySurface, NULL, pBackBuffer, NULL));
			SAFE_RELEASE(pBackBuffer);
		done:
			mpD3D9Device->EndScene();
		}
		OSDText(NULL, TEXT("this is a test %d."), GetTickCount());

		mpD3D9Device->Present(NULL, NULL, NULL, NULL);
	}

	return 0;
}

HRESULT D3D9Render::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = E_FAIL;
	D3DLOCKED_RECT dstRect = { 0 };
	RECT rect = { 0 };
	uint8_t *dstDataPtr = NULL;
	uint8_t *srcDataptr = frame->GetDataPtr();
	GetWindowRect(mhWnd, &rect);
	CAutoLock lock(cs);

	CHECK_HR(hr = mPrimerySurface->LockRect(&dstRect, NULL, 0));
	dstDataPtr = (uint8_t*)dstRect.pBits;
	if (frame->GetPixelFormat() == PIXEL_FORMAT_RGB24){
		for (int i = 0; i < frame->GetHeight(); i++){
			DWORD *rgb32Buffer = rgb32Buffer = (DWORD*)(dstDataPtr + i*dstRect.Pitch);
			uint8_t* rgb24Buffer = frame->GetDataPtr() + frame->GetLineSize()*(frame->GetHeight() - i);
			for (int j = 0; j < frame->GetWidth(); j++){
				rgb32Buffer[j] = RGB(rgb24Buffer[0], rgb24Buffer[1], rgb24Buffer[2]);
				rgb24Buffer += 3;
			}
		}
	} else{
		memcpy(dstDataPtr, frame->GetDataPtr(), frame->GetDataSize());
	}
	CHECK_HR(hr = mPrimerySurface->UnlockRect());

done:
	GetErrorString(hr);

	return hr;
}

BOOL D3D9Render::OSDText(HDC, TCHAR *format, ...)
{
	HRESULT hr = S_OK;
	RECT FontPos;
	TCHAR buf[1024] = { '\0' };
	va_list va_alist;

	GetClientRect(mhWnd, &FontPos);

	va_start(va_alist, format);
	vswprintf_s(buf, format, va_alist);
	va_end(va_alist);

	//CHECK_HR(hr = mPD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0));
	CHECK_HR(hr = mpD3D9Device->BeginScene());
	hr = mPFont->DrawText(NULL, buf, -1, &FontPos,  DT_CENTER, D3DCOLOR_ARGB(255, 0, 255, 0));
	CHECK_HR(hr = mpD3D9Device->EndScene());
	

done:
	GetErrorString(hr);
	return hr == S_OK;
}

const TCHAR* D3D9Render::GetErrorString(HRESULT hr)
{
	const COMERROR *prr = &d3d9ErrorList[D3D_OK];

	if (FAILED(hr)){
		for (int i = 0; i < ARRAYSIZE(d3d9ErrorList); i++){
			if (d3d9ErrorList[i].hr == hr){
				prr = &d3d9ErrorList[i];
			}
		}
	}

	return prr->desc;
}