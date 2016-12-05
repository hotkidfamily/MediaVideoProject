#include "stdafx.h"
#include "D3D9SpriteRender.h"
#include "RenderUtils.h"


D3D9SpriteRender::D3D9SpriteRender()
	: mhWnd(nullptr)
	, mpD3D9OBj(nullptr)
	, mpD3D9Device(nullptr)
	, mPFont(nullptr)
	, mpD3D9Texture(nullptr)

	, mRenderEvent(nullptr)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
	, mVppFactory(nullptr)
	, mVpp(nullptr)
{
}

D3D9SpriteRender::~D3D9SpriteRender()
{
}


DWORD WINAPI D3d9SpriteRenderThread(LPVOID args)
{
	D3D9SpriteRender *pRender = (D3D9SpriteRender*)args;
	return pRender->RenderLoop();
}

void D3D9SpriteRender::SetupMatrices()
{
	// setup world matrix
	D3DXMATRIX  matrixScale, matrixRotation, matrixTranslation, matrixWorld;

	D3DXMatrixScaling(&matrixScale, 1.0f, 1.0f, 1.0f); // no scale
	D3DXMatrixRotationZ(&matrixRotation, 0.0); // no z rotation
	//D3DXMatrixRotationX(&matrixRotation, 0.5);
	D3DXMatrixTranslation(&matrixTranslation, 0, 0, 0); // no translate
	D3DXMatrixMultiply(&matrixWorld, &matrixScale, &matrixRotation);
	D3DXMatrixMultiply(&matrixWorld, &matrixWorld, &matrixTranslation);
	mSprite->SetTransform(&matrixWorld);
}

HRESULT D3D9SpriteRender::GetDeviceType(D3DDISPLAYMODE mode)
{
	HRESULT hr = S_OK;

	mD3D9DeviceType = D3DDEVTYPE_HAL;
	hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, mode.Format, TRUE);
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_SW;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, mode.Format, TRUE);
	}

	return hr;
}

BOOL D3D9SpriteRender::IfSupportedConversionFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat)
{
	HRESULT hr = S_OK;
	mD3D9DeviceType = D3DDEVTYPE_HAL;
	hr = mpD3D9OBj->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, mD3D9DeviceType, pixelFormat, mode.Format);
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_SW;
		hr = mpD3D9OBj->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, mD3D9DeviceType, pixelFormat, mode.Format);
	}

	return hr == S_OK;
}

HRESULT D3D9SpriteRender::IfSupportedFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat)
{
	HRESULT hr = S_OK;

	mD3D9DeviceType = D3DDEVTYPE_HAL;
	hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, pixelFormat, TRUE);
	if (FAILED(hr)){
		mD3D9DeviceType = D3DDEVTYPE_SW;
		hr = mpD3D9OBj->CheckDeviceType(D3DADAPTER_DEFAULT, mD3D9DeviceType, mode.Format, pixelFormat, TRUE);
	}

	return hr == S_OK;
}

BOOL D3D9SpriteRender::InitRender(HWND hWnd, int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = S_OK;
	RECT rect = { 0 };
	D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device
	D3DDISPLAYMODE mode;
	BOOL bNeedConversion = FALSE;

	mhWnd = hWnd;

	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = TRUE; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = mhWnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 1; //set it to only use 1 back buffer
	d3dpp.BackBufferWidth = width; //set the buffer to our window width
	d3dpp.BackBufferHeight = height; //set the buffer to out window height
	d3dpp.BackBufferFormat = GetD3D9PixelFmtByFourCC(pixelFormatInFourCC);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //SwapEffect
	d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_VIDEO;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // wait for VSync
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	mpD3D9OBj = Direct3DCreate9(D3D_SDK_VERSION); //Create the presentation parameters
	mpD3D9OBj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	if (!IfSupportedFormat(mode, d3dpp.BackBufferFormat)){
		if (!IfSupportedConversionFormat(mode, d3dpp.BackBufferFormat)){
			if (GetVPPFactoryObj(mVppFactory)) {
				mVppFactory->CreateVPPObj(mVpp);
			}
			vppParams.srcWidth = width;
			vppParams.srcHeight = height;
			vppParams.srcPixelInFormatFourCC = pixelFormatInFourCC;
			vppParams.dstWidth = width;
			vppParams.dstHeight = height;
			vppParams.dstPixelInFormatFourCC = GetFourCCByD3D9PixelFmt(mode.Format);
			vppParams.flags = SWS_POINT;
			if (!mVpp->InitContext(vppParams)){
				hr = E_FAIL;
				goto done;
			}
			transSampleBuffer = new CSampleBuffer;
			int32_t size = width*height * 4;
			uint8_t *buff = new uint8_t[size];
			transSampleBuffer->Reset(buff, size);
			FRAME_DESC desc;
			desc.dataPtr = 0;
			desc.pixelFormatInFourCC = vppParams.dstPixelInFormatFourCC;
			desc.width = width;
			desc.height = height;
			transSampleBuffer->FillData(desc);
			CHECK_HR(hr = GetDeviceType(mode));
		} else {
			bNeedConversion = TRUE;
			d3dpp.BackBufferFormat = mode.Format;
		}
	}

	mpD3D9OBj->GetDeviceCaps(D3DADAPTER_DEFAULT, mD3D9DeviceType, &mpD3D9DeviceCaps);

	DWORD devBehaviorFlags = 0;
	if (mpD3D9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	CHECK_HR(hr = mpD3D9OBj->CreateDevice(D3DADAPTER_DEFAULT, mD3D9DeviceType, mhWnd, devBehaviorFlags, &d3dpp, &mpD3D9Device));

	if (bNeedConversion)
		d3dpp.BackBufferFormat = GetD3D9PixelFmtByFourCC(pixelFormatInFourCC);

	CHECK_HR(hr = mpD3D9Device->CreateTexture(width, height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture, NULL));
	CHECK_HR(hr = mpD3D9Device->CreateTexture(width, height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture2, NULL));

	CHECK_HR(hr = D3DXCreateFont(mpD3D9Device, 30, 0, FW_LIGHT, 1, TRUE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

	CHECK_HR(hr = D3DXCreateSprite(mpD3D9Device, &mSprite));

	SetupMatrices();

	InitializeCriticalSection(&cs);
	mpReadyTexture = mpD3D9Texture;
	mpFreeTexture = mpD3D9Texture2;

	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("sprite Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mSupportVSync = FALSE;
	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(nullptr, 0, D3d9SpriteRenderThread, this, NULL, &mRenderThreadId);

done:
	if (FAILED(hr)){
		DeinitRender();
		GetD3D9ErrorString(hr);
	}

	return hr == S_OK;
}

BOOL D3D9SpriteRender::DeinitRender()
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

	DeleteCriticalSection(&cs);

	SAFE_RELEASE(mpD3D9Texture);
	SAFE_RELEASE(mpD3D9Texture2);
	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mSprite);
	SAFE_RELEASE(mpD3D9Device);
	SAFE_RELEASE(mpD3D9OBj);

	return TRUE;
}

DWORD D3D9SpriteRender::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = 0;

	while (mRenderThreadRuning){
		dwRet = WaitForSingleObject(mRenderEvent, 10);

		if (!mRenderThreadRuning)
			break;

		if ( dwRet == WAIT_OBJECT_0 ){
			if (SUCCEEDED(mpD3D9Device->BeginScene())){
				if (SUCCEEDED(mSprite->Begin(D3DXSPRITE_ALPHABLEND))){
					hr = mSprite->Draw(mpReadyTexture, NULL, NULL, &D3DXVECTOR3(0, 0, 0), 0XFFFFFFFF);
					mSprite->End();
				}
				mpD3D9Device->EndScene();
			}

			OSDText(nullptr, TEXT("this is a test %d."), GetTickCount());
			EnterCriticalSection(&cs);
			hr = mpD3D9Device->Present(nullptr, nullptr, nullptr, nullptr);
			LeaveCriticalSection(&cs);

		} else if ( dwRet == WAIT_TIMEOUT ){
			continue;
		} else {
			break;
		}
	}

	return 0;
}

BOOL D3D9SpriteRender::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = E_FAIL;
	D3DLOCKED_RECT dstRect = { 0 };
	uint8_t *dstDataPtr = nullptr;
	uint8_t *srcDataptr = nullptr;
	int32_t frameWidth = 0;
	int32_t frameHeight = 0;
	volatile LPDIRECT3DTEXTURE9 pCur = NULL;
	int32_t srcLineSize = 0;

	if (!frame){
		return FALSE;
	}

	frameWidth = frame->GetWidth();
	frameHeight = frame->GetHeight();
	srcDataptr = frame->GetDataPtr();
	srcLineSize = frame->GetLineSize();

	if (SUCCEEDED(hr = mpFreeTexture->LockRect(0, &dstRect, NULL, 0))){
		dstDataPtr = (uint8_t*)dstRect.pBits;
		if (dstRect.Pitch == srcLineSize){
			memcpy(dstDataPtr, srcDataptr, frame->GetDataSize());
		} else{
			if (frame->GetPixelFormat() == PIXEL_FORMAT_RGB24){
				for (int i = 0; i < frameHeight; i++){
					uint8_t *rgb32Buffer = (uint8_t*)(dstDataPtr + i*dstRect.Pitch);
					uint8_t* rgb24Buffer = srcDataptr + srcLineSize*(frameHeight - i);
					for (int j = 0; j < frameWidth; j++){
						rgb32Buffer[0] = rgb24Buffer[0];
						rgb32Buffer[1] = rgb24Buffer[1];
						rgb32Buffer[2] = rgb24Buffer[2];
						rgb32Buffer += 4;
						rgb24Buffer += 3;
					}
				}
			}
		}
		mpFreeTexture->UnlockRect(0);

		EnterCriticalSection(&cs);
		pCur = mpReadyTexture;
		mpReadyTexture = mpFreeTexture;
		mpFreeTexture = pCur;
		LeaveCriticalSection(&cs);

		SetEvent(mRenderEvent);
	}

	if (FAILED(hr))
		GetD3D9ErrorString(hr);

	return hr != DD_OK;
}

BOOL D3D9SpriteRender::OSDText(HDC, TCHAR *format, ...)
{
	HRESULT hr = S_OK;
	RECT FontPos;
	TCHAR buf[1024] = { TEXT('\0') };
	va_list va_alist;

	GetClientRect(mhWnd, &FontPos);

	va_start(va_alist, format);
	vswprintf_s(buf, format, va_alist);
	va_end(va_alist);

	hr = mPFont->DrawText(nullptr, buf, -1, &FontPos, DT_CENTER, D3DCOLOR_ARGB(255, 0, 255, 0));

	if (FAILED(hr))
		GetD3D9ErrorString(hr);

	return hr == S_OK;
}

