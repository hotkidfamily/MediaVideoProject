#include "stdafx.h"
#include "D3D9Render.h"
#include <stdlib.h>

#pragma comment(lib, "videoprocess.lib")

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
	: mhWnd(nullptr)
	, mpD3D9OBj(nullptr)
	, mpD3D9Device(nullptr)
	, mPFont(nullptr)
	, mPrimerySurface(nullptr)
	, mPrimeryTexture(nullptr)

	, mRenderEvent(nullptr)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
	, mVppFactory(nullptr)
	, mVpp(nullptr)
{
}

D3D9Render::~D3D9Render()
{
}


DWORD WINAPI D3d9RenderThread(LPVOID args)
{
	D3D9Render *pRender = (D3D9Render*)args;
	return pRender->RenderLoop();
}

void D3D9Render::SetupMatrices()
{
	// setup world matrix
	D3DXMATRIX  worldMatrix;
	D3DXMatrixIdentity(&worldMatrix);
	mpD3D9Device->SetTransform(D3DTS_WORLD, &worldMatrix);

	// setup view matrix
	D3DXVECTOR3 eyeVect(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 curVect(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 upVect(0.0f, 0.0f, -10.0f);

	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &eyeVect, &curVect, &upVect);

	// setup projection matrix
	D3DXMATRIX projectMatrix;
	D3DXMatrixPerspectiveFovLH(&projectMatrix, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	mpD3D9Device->SetTransform(D3DTS_PROJECTION, &projectMatrix);
}

HRESULT D3D9Render::GetDisplayMode()
{
	D3DDISPLAYMODE mode;
	HRESULT hr = S_OK;

	mpD3D9OBj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	mD3D9DeviceType = D3DDEVTYPE_HAL;
	hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, mode.Format, TRUE);

	// software device
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_SW;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, mode.Format, TRUE);
	}

	// reference rasterizer device
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_REF;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, mode.Format, TRUE);
	}

	return hr;
}

// check windowed mode color convert ability
HRESULT D3D9Render::IfSupportedFormat(D3DFORMAT pixelFormat)
{
	HRESULT hr = S_OK;
	D3DDISPLAYMODE mode;

	mpD3D9OBj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	mD3D9DeviceType = D3DDEVTYPE_HAL;
	hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, pixelFormat, TRUE);
	if (FAILED(hr)){
		hr = mpD3D9OBj->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, mD3D9DeviceType, pixelFormat, mode.Format);
	}

	// software device
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_SW;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, pixelFormat, TRUE);
		if (FAILED(hr)){
			hr = mpD3D9OBj->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, mD3D9DeviceType, pixelFormat, mode.Format);
		}
	}
	
	// reference rasterizer device
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_REF;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, pixelFormat, TRUE);
		if (FAILED(hr)){
			hr = mpD3D9OBj->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, mD3D9DeviceType, pixelFormat, mode.Format);
		}
	}
	
	return hr;
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
	case PIXEL_FORMAT_YV12:
	case PIXEL_FORMAT_I420:
		*pd3dPixelFormat = (D3DFORMAT)dwFourCC;
		break;
	default:
		*pd3dPixelFormat = D3DFMT_UNKNOWN;
		break;
	}
}

BOOL D3D9Render::InitRender(HWND hWnd, int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = S_OK;
	RECT rect = { 0 };
	D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device

	mhWnd = hWnd;

	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = TRUE; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = mhWnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 3; //set it to only use 1 back buffer
	d3dpp.BackBufferWidth = width; //set the buffer to our window width
	d3dpp.BackBufferHeight = height; //set the buffer to out window height
	FourCCtoD3DFormat(&d3dpp.BackBufferFormat, pixelFormatInFourCC);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //SwapEffect
	d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_VIDEO;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // wait for VSync
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	
	mpD3D9OBj = Direct3DCreate9(D3D_SDK_VERSION); //Create the presentation parameters

	if (FAILED(hr = IfSupportedFormat(d3dpp.BackBufferFormat))){
		// Create VPP Support
		if (GetVPPFactoryObj(mVppFactory)) {
			mVppFactory->CreateVPPObj(mVpp);
		}
		vppParams.srcWidth = width;
		vppParams.srcHeight = height;
		vppParams.srcPixelInFormatFourCC = pixelFormatInFourCC;
		vppParams.dstWidth = width;
		vppParams.dstHeight = height;
		vppParams.dstPixelInFormatFourCC = PIXEL_FORMAT_RGB32;
		vppParams.flags = SWS_POINT;
		if (!mVpp->InitContext(vppParams)){
			hr = E_FAIL;
			goto done;
		}
		CHECK_HR(hr = GetDisplayMode());
	}

	mpD3D9OBj->GetDeviceCaps(D3DADAPTER_DEFAULT, mD3D9DeviceType, &mpD3D9DeviceCaps);

	DWORD devBehaviorFlags = 0;
	if (mpD3D9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	CHECK_HR(hr = mpD3D9OBj->CreateDevice(D3DADAPTER_DEFAULT, mD3D9DeviceType, mhWnd, devBehaviorFlags, &d3dpp, &mpD3D9Device));
	CHECK_HR(hr = D3DXCreateFont(mpD3D9Device, 30, 0, FW_LIGHT, 1, TRUE, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

	CHECK_HR(hr = mpD3D9Device->CreateOffscreenPlainSurface(width, height, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &mPrimerySurface, nullptr));
	// CHECK_HR(hr = mpD3D9Device->CreateTexture(width, height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &mPrimeryTexture, nullptr));

	// SetupMatrices();
	// CHECK_HR(hr = mpD3D9Device->SetRenderState(D3DRS_LIGHTING, FALSE));

	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mSupportVSync = FALSE;
	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(nullptr, 0, D3d9RenderThread, this, NULL, &mRenderThreadId);

done:
	if (FAILED(hr)){
		DeinitRender();
	}

	GetErrorString(hr);

	return hr == S_OK;
}

BOOL D3D9Render::DeinitRender()
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

	if (mVpp){
		mVpp->DeinitContext();
		mVppFactory->DestoryVPPObj(mVpp);
		mVpp = nullptr;
	}

	ReleaseVPPFactoryObj(mVppFactory);
	mVppFactory = nullptr;

	

	SAFE_RELEASE(mPrimerySurface);
	SAFE_RELEASE(mPrimeryTexture);
	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mpD3D9Device);
	SAFE_RELEASE(mpD3D9OBj);

	return TRUE;
}

#define USE_BACKBUFFER 1

DWORD D3D9Render::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = 0;

	while (mRenderThreadRuning){
		dwRet = WaitForSingleObject(mRenderEvent, 20);
		if (dwRet == WAIT_OBJECT_0){

#if USE_BACKBUFFER
#else
			if (SUCCEEDED(hr = mpD3D9Device->BeginScene())){
				IDirect3DSurface9 *pBackBuffer = nullptr;
				CHECK_HR(hr = mpD3D9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer));
				CHECK_HR(hr = mpD3D9Device->UpdateSurface(mPrimerySurface, nullptr, pBackBuffer, nullptr));
				SAFE_RELEASE(pBackBuffer);
			done:
				mpD3D9Device->EndScene();
			}
#endif

			OSDText(nullptr, TEXT("this is a test %d."), GetTickCount());

			hr = mpD3D9Device->Present(nullptr, nullptr, nullptr, nullptr);
		} else if (dwRet == WAIT_TIMEOUT){
			continue;
		}else{
			break;
		}
	}

	return 0;
}

BOOL D3D9Render::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = E_FAIL;
	D3DLOCKED_RECT dstRect = { 0 };
	D3DSURFACE_DESC dstDec;
	uint8_t *dstDataPtr = nullptr;
	uint8_t *srcDataptr = nullptr;
	IDirect3DSurface9 *pSurface = nullptr;
	int32_t frameWidth = 0;
	int32_t frameHeight = 0;
	int32_t srcLineSize = 0;

	if (!frame){
		return FALSE;
	}

	frameWidth = frame->GetWidth();
	frameHeight = frame->GetHeight();
	srcDataptr = frame->GetDataPtr();
	srcLineSize = frame->GetLineSize();

#if USE_BACKBUFFER
	CHECK_HR(hr = mpD3D9Device->GetBackBuffer(0, 2, D3DBACKBUFFER_TYPE_MONO, &pSurface));
#else
	pSurface = mPrimerySurface;
#endif
	pSurface->GetDesc(&dstDec);
	if (SUCCEEDED(hr = pSurface->LockRect(&dstRect, nullptr, 0))){
		dstDataPtr = (uint8_t*)dstRect.pBits;
		if (frame->GetPixelFormat() == PIXEL_FORMAT_RGB24){
			for (int i = 0; i < frameHeight; i++){
				DWORD *rgb32Buffer = rgb32Buffer = (DWORD*)(dstDataPtr + i*dstRect.Pitch);
				uint8_t* rgb24Buffer = srcDataptr  + srcLineSize*(frameHeight - i);
				for (int j = 0; j < frameWidth; j++){
					rgb32Buffer[j] = RGB(rgb24Buffer[0], rgb24Buffer[1], rgb24Buffer[2]);
					rgb24Buffer += 3;
				}
			}
		} else{
			memcpy(dstDataPtr, srcDataptr, frame->GetDataSize());
		}
		hr = pSurface->UnlockRect();
#if USE_BACKBUFFER
		SAFE_RELEASE(pSurface);
#endif
		SetEvent(mRenderEvent);
	}

done:
	GetErrorString(hr);

	return hr != DD_OK;
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

	//CHECK_HR(hr = mPD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0));
	if (SUCCEEDED(hr = mpD3D9Device->BeginScene())){
		mPFont->DrawText(nullptr, buf, -1, &FontPos, DT_CENTER, D3DCOLOR_ARGB(255, 0, 255, 0));
		mpD3D9Device->EndScene();
	}

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