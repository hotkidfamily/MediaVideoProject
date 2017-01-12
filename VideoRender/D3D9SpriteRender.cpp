#include "stdafx.h"
#include "D3D9SpriteRender.h"
#include "RenderUtils.h"
#include "logger.h"

#include <timeapi.h>

#define FONT_HEIGHT 30


D3D9SpriteRender::D3D9SpriteRender()
	: mhWnd(nullptr)
	, mpD3D9OBj(nullptr)
	, mpD3D9Device(nullptr)
	, mPFont(nullptr)
	, mCurPushObjIndex(0)
	, mCurRenderObjIndex(MAX_RENDER_OBJ-1)
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
	ZeroMemory(mpD3D9Texture, sizeof(mpD3D9Texture));
	ZeroMemory(mpD3D9Surface, sizeof(mpD3D9Surface));
}

D3D9SpriteRender::~D3D9SpriteRender()
{
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

BOOL D3D9SpriteRender::InitRender(const RENDERCONFIG &config)
{
	HRESULT hr = S_OK;
	RECT rect = { 0 };
	D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device
	D3DDISPLAYMODE mode;

	mConfig = config;
	mhWnd = config.hWnd;

	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = TRUE; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = mhWnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 1; //set it to only use 1 back buffer
	d3dpp.BackBufferWidth = config.width; //set the buffer to our window width
	d3dpp.BackBufferHeight = config.height; //set the buffer to out window height
	d3dpp.BackBufferFormat = GetD3D9PixelFmtByFourCC(config.pixelFormat);
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP; //SwapEffect
	d3dpp.Flags |= D3DPRESENTFLAG_VIDEO;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // wait for VSync
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	CHECK_HR((hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &mpD3D9OBj))); //Create the presentation parameters
	CHECK_HR((hr = mpD3D9OBj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode)));

	if (!IfSupportedFormat(mode, d3dpp.BackBufferFormat)){
		if (!IfSupportedConversionFormat(mode, d3dpp.BackBufferFormat)){
			d3dpp.BackBufferFormat = mode.Format;
			if (GetVPPFactoryObj(mVppFactory)) {
				mVppFactory->CreateVPPObj(mVpp);
			}
			vppParams.srcWidth = config.width;
			vppParams.srcHeight = config.height;
			vppParams.srcPixelInFormatFourCC = config.pixelFormat;
			vppParams.dstWidth = config.width;
			vppParams.dstHeight = config.height;
			vppParams.dstPixelInFormatFourCC = GetFourCCByD3D9PixelFmt(mode.Format);
			vppParams.flags = 0x10;
			if (!mVpp->InitContext(vppParams)){
				hr = E_FAIL;
				goto done;
			}
			mVppTransSampleBuffer = AllocSampleBuffer(config.width, config.height, (CPPixelFormat)vppParams.dstPixelInFormatFourCC);
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

	DWORD devBehaviorFlags = D3DCREATE_MULTITHREADED;
	if (mpD3D9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	CHECK_HR(hr = mpD3D9OBj->CreateDeviceEx(D3DADAPTER_DEFAULT, mD3D9DeviceType, mhWnd, devBehaviorFlags, &d3dpp, FALSE, &mpD3D9Device));

	if (mbSupportConversion)
		d3dpp.BackBufferFormat = GetD3D9PixelFmtByFourCC(config.pixelFormat);

	mSupportSurfaceType = SUPPORT_TEXTURE; /* texture */
	if (FAILED(hr = mpD3D9Device->CreateTexture(config.width, config.height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture[0], NULL))) {
		if (FAILED(hr = mpD3D9Device->CreateOffscreenPlainSurface(config.width, config.height, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &mpD3D9Surface[0], NULL))) {
			goto done;
		} else{
			for (int i = 1; i < MAX_RENDER_OBJ; i++){
				hr = mpD3D9Device->CreateOffscreenPlainSurface(config.width, config.height, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &mpD3D9Surface[i], NULL);
			}
			mSupportSurfaceType = SUPPORT_SURFACE; /* surface */
		}
	} else{
		for (int i = 1; i < MAX_RENDER_OBJ; i++){
			hr = mpD3D9Device->CreateTexture(config.width, config.height, 0, 0, d3dpp.BackBufferFormat, D3DPOOL_MANAGED, &mpD3D9Texture[i], NULL);
		}
	}

	// indicate to 0
	mCurPushObjIndex = 0;

	CHECK_HR(hr = D3DXCreateFont(mpD3D9Device, FONT_HEIGHT, 0, FW_LIGHT, 1, TRUE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

	CHECK_HR(hr = D3DXCreateSprite(mpD3D9Device, &mSprite));
	
	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("sprite Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}

	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(nullptr, 0, RenderThread, this, NULL, &mRenderThreadId);

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

	for (int i = 0; i < MAX_RENDER_OBJ; i++){
		SAFE_RELEASE(mpD3D9Texture[i]);
		SAFE_RELEASE(mpD3D9Surface[i]);
	}

	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mSprite);
	SAFE_RELEASE(mpD3D9Device);
	SAFE_RELEASE(mpD3D9OBj);

	return TRUE;
}

BOOL D3D9SpriteRender::DrawStatus()
{
	RECT FontPos;
	HRESULT hr = S_OK;
	GetClientRect(mhWnd, &FontPos);
	int32_t minInputSample = 0, maxInputSample = 0;
	int32_t minRenderSample = 0, maxRenderSample = 0;

	if (mInputStatis.Samples() > 2 && mRenderStatis.Samples() > 2){
		mInputStatis.MinMaxSample(minInputSample, maxInputSample);
		mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);
		if ((hr = mpD3D9Device->BeginScene()) == D3D_OK ){
			OSDText(NULL, &FontPos,
				TEXT("Frame: %dx%d, %.3f"),
				mConfig.width, mConfig.height, mConfig.fps.num/mConfig.fps.den);

			OSDText(NULL, &FontPos,
				TEXT("FPS: In %.2f, Out %.2f, Current surface: %d"),
				mInputStatis.Frequency(), mRenderStatis.Frequency(), mCurPushObjIndex);

			OSDText(NULL, &FontPos,
				TEXT("Input: %2lld, Avg:%2llu(%2d~%2d)"),
				mCurPtsInterval, mInputStatis.AvgSampleSize(), minInputSample, maxInputSample);

			OSDText(NULL, &FontPos,
				TEXT("Render(%s): %2d Avg:%2llu(%2d~%2d)"), 
				(mSupportSurfaceType == SUPPORT_SURFACE) ? _T("SURFACE") : _T("TEXTTURE"),
				mCurRenderInterval, mRenderStatis.AvgSampleSize(), minRenderSample, maxRenderSample);

			mpD3D9Device->EndScene();
		}
	}

	return TRUE;
}

BOOL D3D9SpriteRender::UpdateRenderStatis()
{
	DWORD renderBefore = timeGetTime();

	if (mLastRender){
		mCurRenderInterval = renderBefore - mLastRender;
		mRenderStatis.AppendSample(mCurRenderInterval);
	}

	mLastRender = renderBefore;

	return FALSE;
}

DWORD D3D9SpriteRender::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = WAIT_OBJECT_0;
	int32_t step = 0;
	int32_t lastSurface = 0;
	LPDIRECT3DSURFACE9 pCurSurface = NULL;
	LPDIRECT3DTEXTURE9 pCurTexture = NULL;

	while (mRenderThreadRuning){
		hr = mpD3D9Device->WaitForVBlank(0);
		if (FAILED(hr)){
			dwRet = WaitForSingleObject(mRenderEvent, 10);
		}

		if (!mRenderThreadRuning)
			break;

		if (dwRet == WAIT_OBJECT_0){
			UpdateRenderStatis(); 

			if (step++ >= 1){

				if (lastSurface != mCurPushObjIndex){
					mCurRenderObjIndex = (mCurRenderObjIndex + 1) % MAX_RENDER_OBJ;
					step = 0;
				}

				lastSurface = mCurPushObjIndex;
			}

			internel_log(Info, "render %d, push %d, %d \n", mCurRenderObjIndex, mCurPushObjIndex, step);

			if (mSupportSurfaceType == SUPPORT_TEXTURE){
				pCurTexture = mpD3D9Texture[mCurRenderObjIndex];
				if (!SUCCEEDED(pCurTexture->GetSurfaceLevel(0, &pCurSurface))) {
					continue;
				}
			} else{
				pCurSurface = mpD3D9Surface[mCurRenderObjIndex];
			}

			IDirect3DSurface9 *pBackBuffer = nullptr;
			if (SUCCEEDED(mpD3D9Device->BeginScene())) {
				if (SUCCEEDED(mpD3D9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer))) {
					mpD3D9Device->StretchRect(pCurSurface, NULL, pBackBuffer, NULL, D3DTEXF_NONE);
					pBackBuffer->Release();
				}
				mpD3D9Device->EndScene();
			}

			DrawStatus();

			if ((hr = mpD3D9Device->Present(nullptr, nullptr, nullptr, nullptr)) != D3D_OK){
			//	GetD3D9ErrorString(hr);
			}
		} else if ( dwRet == WAIT_TIMEOUT ){
			continue;
		} else {
			break;
		}
	}

	return 0;
}

HRESULT D3D9SpriteRender::UpdateRenderSurface(CSampleBuffer *&frame)
{
	HRESULT hr = E_FAIL;
	uint8_t *dstDataPtr = nullptr;
	uint8_t *srcDataptr = nullptr;
	int32_t srcLineSize = 0;
	int32_t frameHeight = 0;
	D3DLOCKED_RECT dstRect = { 0 };
	IDirect3DSurface9 *pCurSurface = NULL;
	IDirect3DTexture9 *pCurTexture = NULL;

	srcDataptr = frame->GetDataPtr();
	srcLineSize = frame->GetLineSize();
	frameHeight = frame->GetHeight();

	if (mSupportSurfaceType == SUPPORT_TEXTURE){
		pCurTexture = mpD3D9Texture[mCurPushObjIndex];
		if (SUCCEEDED(hr = pCurTexture->LockRect(0, &dstRect, NULL, 0))){
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
			pCurTexture->UnlockRect(0);
		}
	} else{
		pCurSurface = mpD3D9Surface[mCurPushObjIndex];
		if (SUCCEEDED(hr = pCurSurface->LockRect(&dstRect, NULL, 0))){
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
			pCurSurface->UnlockRect();
		}
	}

	//internel_log(Info, "push %d\n", mCurPushObjIndex);

	mCurPushObjIndex = (mCurPushObjIndex + 1) % MAX_RENDER_OBJ;

	return hr;
};

BOOL D3D9SpriteRender::UpdatePushStatis(CSampleBuffer *&frame)
{
	int64_t ptss = 0, ptse = 0;

	frame->GetPts(ptss, ptse);
	if (mLastPts){
		mCurPtsInterval = ptss - mLastPts;
		mInputStatis.AppendSample((int32_t)mCurPtsInterval);
	}
	mLastPts = ptss;

	return TRUE;
}

BOOL D3D9SpriteRender::PushFrame(CSampleBuffer *inframe)
{
	HRESULT hr = E_FAIL;
	CSampleBuffer *frame = inframe;
	
	if (!frame){
		return FALSE;
	}

	if (mbNeedVpp){
		mVpp->ProcessFrame(inframe, mVppTransSampleBuffer);
		frame = mVppTransSampleBuffer;
	}

	hr = UpdateRenderSurface(frame);

	UpdatePushStatis(frame);

	SetEvent(mRenderEvent);

	if (FAILED(hr))
		GetD3D9ErrorString(hr);

	return hr != D3D_OK;
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

