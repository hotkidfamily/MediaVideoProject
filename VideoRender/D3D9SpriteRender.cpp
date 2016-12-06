#include "stdafx.h"
#include "D3D9SpriteRender.h"
#include "RenderUtils.h"

#define FONT_HEIGHT 30


D3D9SpriteRender::D3D9SpriteRender()
	: mhWnd(nullptr)
	, mpD3D9OBj(nullptr)
	, mpD3D9Device(nullptr)
	, mPFont(nullptr)
	, mpD3D9Texture(nullptr)
	, mpD3D9Texture2(nullptr)
	, mpD3D9Surface(nullptr)
	, mpD3D9Surface2(nullptr)
	, mbSupportConversion(FALSE)
	, mSupportSurfaceType(SUPPORT_TEXTURE)

	, mRenderEvent(nullptr)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)

	, mVppFactory(nullptr)
	, mVpp(nullptr)
	, mVppTransSampleBuffer(NULL)
	, mbNeedVpp(FALSE)

	, mCurRenderInterval(0)
	, mCurPtsInterval(0)
	, mLastRender(0)
	, mLastPts(0)
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
	HRESULT hr = E_FAIL;
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
			d3dpp.BackBufferFormat = mode.Format;
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
			mVppTransSampleBuffer = AllocSampleBuffer(width, height, (CPPixelFormat)vppParams.dstPixelInFormatFourCC);
			if (mVppTransSampleBuffer){
				mbNeedVpp = TRUE;
			}
			
			CHECK_HR(hr = GetDeviceType(mode));
		} else {
			mbSupportConversion = TRUE;
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

	if (mbSupportConversion)
		d3dpp.BackBufferFormat = GetD3D9PixelFmtByFourCC(pixelFormatInFourCC);

	mSupportSurfaceType = SUPPORT_TEXTURE; /* texture */
	if (FAILED(hr = mpD3D9Device->CreateTexture(width, height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture, NULL))){
		if (FAILED(hr = mpD3D9Device->CreateOffscreenPlainSurface(width, height, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &mpD3D9Surface, NULL))){
			goto done;
		} else{
			hr = mpD3D9Device->CreateOffscreenPlainSurface(width, height, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &mpD3D9Surface2, NULL);
			mSupportSurfaceType = SUPPORT_SURFACE; /* surface */
		}
	} else{
		hr = mpD3D9Device->CreateTexture(width, height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture2, NULL);
	}

	CHECK_HR(hr = D3DXCreateFont(mpD3D9Device, FONT_HEIGHT, 0, FW_LIGHT, 1, TRUE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

	CHECK_HR(hr = D3DXCreateSprite(mpD3D9Device, &mSprite));

	SetupMatrices();

	InitializeCriticalSection(&cs);

	if (mSupportSurfaceType == SUPPORT_TEXTURE ){
		mpReadyObj = mpD3D9Texture;
		mpFreeObj = mpD3D9Texture2;
	}else{
 		mpReadyObj = mpD3D9Surface;
 		mpFreeObj = mpD3D9Surface2;
	}
	
	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("sprite Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

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
	}

	if (mVpp){
		mVpp->DeinitContext();
		mVppFactory->DestoryVPPObj(mVpp);
		mVpp = nullptr;
	}

	ReleaseVPPFactoryObj(mVppFactory);
	mVppFactory = nullptr;

	if (mVppTransSampleBuffer){
		DeallocSampleBuffer(mVppTransSampleBuffer);
		mVppTransSampleBuffer = NULL;
	}

	DeleteCriticalSection(&cs);

	SAFE_RELEASE(mpD3D9Surface);
	SAFE_RELEASE(mpD3D9Surface2);
	SAFE_RELEASE(mpD3D9Texture);
	SAFE_RELEASE(mpD3D9Texture2);
	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mSprite);
	SAFE_RELEASE(mpD3D9Device);
	SAFE_RELEASE(mpD3D9OBj);

	return TRUE;
}

BOOL D3D9SpriteRender::OutputInformation()
{
	RECT FontPos;
	GetClientRect(mhWnd, &FontPos);
	int32_t minInputSample = 0, maxInputSample = 0;
	int32_t minRenderSample = 0, maxRenderSample = 0;

	if (mInputStatis.Samples() > 2 && mRenderStatis.Samples() > 2){
		mInputStatis.MinMaxSample(minInputSample, maxInputSample);
		mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);

		OSDText(NULL, &FontPos, 
			TEXT("FPS: %.2f"),
			mInputStatis.Frequency());

		OSDText(NULL, &FontPos,
			TEXT("Input: %2lld, Avg:%2llu(%2d~%2d)"),
			mCurPtsInterval, mInputStatis.AvgSampleSize(), minInputSample, maxInputSample);

		OSDText(NULL, &FontPos,
			TEXT("Render: %2d Avg:%2llu(%2d~%2d)"),
			mCurRenderInterval, mRenderStatis.AvgSampleSize(), minRenderSample, maxRenderSample);
	}

	return TRUE;
}

DWORD D3D9SpriteRender::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = 0;
	DWORD renderBefore = 0;
	mCurRenderInterval = 0;

	while (mRenderThreadRuning){
		dwRet = WaitForSingleObject(mRenderEvent, 10);

		if (!mRenderThreadRuning)
			break;

		if ( dwRet == WAIT_OBJECT_0 ){
			renderBefore = timeGetTime();

			if (mLastRender){
				mCurRenderInterval = renderBefore - mLastRender;
				mRenderStatis.AppendSample(mCurRenderInterval);
			}
			mLastRender = renderBefore;
			if (SUCCEEDED(mpD3D9Device->BeginScene())){
				if (mSupportSurfaceType == SUPPORT_TEXTURE){
					if (SUCCEEDED(mSprite->Begin(D3DXSPRITE_ALPHABLEND))){
						hr = mSprite->Draw((LPDIRECT3DTEXTURE9)mpReadyObj, NULL, NULL, &D3DXVECTOR3(0, 0, 0), 0XFFFFFFFF);
						mSprite->End();
					}
				} else{
					IDirect3DSurface9 *pBackBuffer = nullptr;
					if (SUCCEEDED(hr = mpD3D9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer))){
						mpD3D9Device->StretchRect((IDirect3DSurface9*)mpReadyObj, NULL, pBackBuffer, NULL, D3DTEXF_NONE);
						pBackBuffer->Release();
					}
				}
				mpD3D9Device->EndScene();
			}

			OutputInformation();

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

HRESULT D3D9SpriteRender::updateContent(CSampleBuffer *&frame)
{
	HRESULT hr = E_FAIL;
	uint8_t *dstDataPtr = nullptr;
	uint8_t *srcDataptr = nullptr;
	int32_t frameWidth = 0;
	int32_t srcLineSize = 0;
	int32_t frameHeight = 0;
	D3DLOCKED_RECT dstRect = { 0 };

	srcDataptr = frame->GetDataPtr();
	srcLineSize = frame->GetLineSize();
	frameWidth = frame->GetWidth();
	frameHeight = frame->GetHeight();

	if (mSupportSurfaceType == SUPPORT_TEXTURE)
	{
		if (SUCCEEDED(hr = ((LPDIRECT3DTEXTURE9)mpFreeObj)->LockRect(0, &dstRect, NULL, 0))){
			dstDataPtr = (uint8_t*)dstRect.pBits;
			if (dstRect.Pitch == srcLineSize){
				memcpy(dstDataPtr, srcDataptr, frame->GetDataSize());
			} else{
				for (int i = 0; i < frameHeight; i++){
					uint8_t *dstlineBuffer = (uint8_t*)(dstDataPtr + i*dstRect.Pitch);
					uint8_t* srcLineBuffer = srcDataptr + srcLineSize*i;
					memcpy_s(dstlineBuffer, dstRect.Pitch, srcLineBuffer, srcLineSize);
				}
			}
			((LPDIRECT3DTEXTURE9)mpFreeObj)->UnlockRect(0);
		}
	} else{
		if (SUCCEEDED(hr = ((IDirect3DSurface9*)mpFreeObj)->LockRect(&dstRect, NULL, 0))){
			dstDataPtr = (uint8_t*)dstRect.pBits;
			if (dstRect.Pitch == srcLineSize){
				memcpy(dstDataPtr, srcDataptr, frame->GetDataSize());
			} else{
				for (int i = 0; i < frameHeight; i++){
					uint8_t *dstlineBuffer = (uint8_t*)(dstDataPtr + i*dstRect.Pitch);
					uint8_t* srcLineBuffer = srcDataptr + srcLineSize*i;
					memcpy_s(dstlineBuffer, dstRect.Pitch, srcLineBuffer, srcLineSize);
				}
			}
			((IDirect3DSurface9*)mpFreeObj)->UnlockRect();
		}
	}

	return hr;
};

BOOL D3D9SpriteRender::PushFrame(CSampleBuffer *inframe)
{
	HRESULT hr = E_FAIL;
	volatile LPVOID pCur = NULL;
	CSampleBuffer *frame = inframe;
	int64_t ptss = 0, ptse = 0;
	
	if (!frame){
		return FALSE;
	}

	frame->GetPts(ptss, ptse);

	if (mLastPts){
		mCurPtsInterval = ptss - mLastPts;
		mInputStatis.AppendSample(mCurPtsInterval);
	}
	
	mLastPts = ptss;

	if (mbNeedVpp){
		mVpp->ProcessFrame(inframe, mVppTransSampleBuffer);
		frame = mVppTransSampleBuffer;
	}

	hr = updateContent(frame);
	
	if (SUCCEEDED(hr)){
		EnterCriticalSection(&cs);
		pCur = mpReadyObj;
		mpReadyObj = mpFreeObj;
		mpFreeObj = pCur;
		LeaveCriticalSection(&cs);

		SetEvent(mRenderEvent);
	}

	if (FAILED(hr))
		GetD3D9ErrorString(hr);

	return hr != DD_OK;
}

BOOL D3D9SpriteRender::OSDText(HDC, RECT *rc, TCHAR *format, ...)
{
	HRESULT hr = S_OK;
	TCHAR buf[1024] = { TEXT('\0') };
	va_list va_alist;

	va_start(va_alist, format);
	vswprintf_s(buf, format, va_alist);
	va_end(va_alist);

	OffsetRect(rc, 0, 2);

	hr = mPFont->DrawText(nullptr, buf, -1, rc, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 0, 255, 0));

	if (FAILED(hr))
		GetD3D9ErrorString(hr);

	rc->top += FONT_HEIGHT;

	return hr == S_OK;
}

