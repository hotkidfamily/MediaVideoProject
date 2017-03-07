#include "stdafx.h"
#include "SyncRender.h"
#include "D3DSoundRender.h"

D3DSoundRender::D3DSoundRender()
	: mRenderEvent(nullptr)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)

	, mCurRenderInterval(0)
	, mCurPtsInterval(0)
	, mLastRender(0)
	, mLastPts(0)

	, mFirstRender(TRUE)
	, mDropFrameCount(0)
{
}

D3DSoundRender::~D3DSoundRender()
{
}

HRESULT D3DSoundRender::GetDeviceType(D3DDISPLAYMODE mode)
{
	HRESULT hr = S_OK;


	return hr;
}

BOOL D3DSoundRender::IfSupportedConversionFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat)
{
	HRESULT hr = E_FAIL;

	return hr == S_OK;
}

HRESULT D3DSoundRender::IfSupportedFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat)
{
	HRESULT hr = S_OK;


	return hr == S_OK;
}

BOOL D3DSoundRender::InitRender(const RENDERCONFIG &config)
{
	HRESULT hr = S_OK;
	
	mRenderEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("Render Event"));
	if (mRenderEvent == INVALID_HANDLE_VALUE){
		hr = E_FAIL;
		goto done;
	}



	mRenderThreadRuning = TRUE;
	mRenderThreadHandle = CreateThread(NULL, 0, CreateRenderThread, (IRenderThread*)this, NULL, &mRenderThreadId);

done:
	if (FAILED(hr)){
		DeinitRender();
		logger(Error, "%s", DXGetErrorStringA(hr));
	}

	return hr == S_OK;
}

BOOL D3DSoundRender::DeinitRender()
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

	return TRUE;
}

BOOL D3DSoundRender::DrawStatus()
{
	HRESULT hr = S_OK;
	int32_t minInputSample = 0, maxInputSample = 0;
	int32_t minRenderSample = 0, maxRenderSample = 0;

	if (mInputStatis.Samples() > 2 && mRenderStatis.Samples() > 2){
		mInputStatis.MinMaxSample(minInputSample, maxInputSample);
		mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);
		
	}

	return TRUE;
}

BOOL D3DSoundRender::UpdateRenderStatis()
{
	DWORD renderBefore = timeGetTime();

	if (mLastRender){
		mCurRenderInterval = renderBefore - mLastRender;
		mRenderStatis.AppendSample(mCurRenderInterval);
	}

	mLastRender = renderBefore;

	return FALSE;
}


DWORD D3DSoundRender::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = WAIT_OBJECT_0;
	LPDIRECT3DSURFACE9 pCurSurface = NULL;
	LPDIRECT3DTEXTURE9 pCurTexture = NULL;

	while (mRenderThreadRuning){
		dwRet = WaitForSingleObject(mRenderEvent, 2);
		switch (dwRet){
		case WAIT_OBJECT_0:
			if (!mRenderThreadRuning){
				logger(Info, "Render thread exit\n");
				break;
			} else{
				UpdateRenderStatis();
				DrawStatus();
			}
			break;
		case WAIT_TIMEOUT:
			continue;
		default:
			logger(Info, "Render thread exit, error Code %x\n", GetLastError());
			break;
		}
	}

	return 0;
}

HRESULT D3DSoundRender::UpdateRenderSurface(CSampleBuffer *&frame)
{
	HRESULT hr = E_FAIL;

	return hr;
};

BOOL D3DSoundRender::UpdatePushStatis(CSampleBuffer *&frame)
{
	if (mLastPts){
		mCurPtsInterval = frame->ptsStart - mLastPts;
		mInputStatis.AppendSample((int32_t)mCurPtsInterval);
	}
	mLastPts = frame->ptsStart;

	return TRUE;
}

BOOL D3DSoundRender::PushFrame(CSampleBuffer *inframe)
{
	HRESULT hr = E_FAIL;
	CSampleBuffer *frame = inframe;
	
	if (!frame){
		return FALSE;
	}

	if (mFirstRender){
		mRenderClock.Reset();
		mFirstRender = FALSE;
	}

	if (mRenderClock.PushFrame(frame)){
		hr = UpdateRenderSurface(frame);

		UpdatePushStatis(frame);

		SetEvent(mRenderEvent);
	} else{
		mDropFrameCount++;
	}

	return hr != D3D_OK;
}

BOOL D3DSoundRender::OSDText(HDC, RECT *rc, TCHAR *format, ...)
{
	HRESULT hr = S_OK;
	TCHAR buf[1024] = { TEXT('\0') };
	va_list va_alist;

	va_start(va_alist, format);
	vswprintf_s(buf, format, va_alist);
	va_end(va_alist);

	OffsetRect(rc, 0, 2);
// 
// 	hr = mPFont->DrawText(nullptr, buf, -1, rc, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 0, 255, 0));

	if (FAILED(hr))
		logger(Error, "%s", DXGetErrorStringA(hr));

	rc->top += 20;

	return hr == S_OK;
}

