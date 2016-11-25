#include "stdafx.h"
#include "D3D9Render.h"


D3D9Render::D3D9Render()
	: mhWnd(NULL)
	, mPD3D9DOBj(NULL)
	, mPD3DDevice(NULL)
	, mPFont(NULL)

	, mRenderEvent(NULL)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
{
}

D3D9Render::D3D9Render(HWND hWnd)
	: mhWnd(hWnd)
	, mPD3D9DOBj(NULL)
	, mPD3DDevice(NULL)
	, mPFont(NULL)

	, mRenderEvent(NULL)
	, mSupportVSync(FALSE)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)
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

HRESULT D3D9Render::InitializeRenderContext(int width, int height, DWORD pixelFormatInFourCC)
{
	HRESULT hr = S_OK;
	D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device
	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = TRUE; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = mhWnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 1; //set it to only use 1 backbuffer
	d3dpp.BackBufferWidth = width; //set the buffer to our window width
	d3dpp.BackBufferHeight = height; //set the buffer to out window height
	d3dpp.BackBufferFormat = (D3DFORMAT)pixelFormatInFourCC; //the backbuffer format
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //SwapEffect

	mPD3D9DOBj = Direct3DCreate9(D3D_SDK_VERSION); //Create the presentation parameters

	CHECK_HR(hr = mPD3D9DOBj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mhWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mPD3DDevice));
	CHECK_HR(hr = D3DXCreateFont(mPD3DDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &mPFont));

done:
	if (FAILED(hr)){
		DeinitRenderContext();
	}

	return hr;
}

HRESULT D3D9Render::DeinitRenderContext()
{
	SAFE_RELEASE(mPFont);
	SAFE_RELEASE(mPD3DDevice);
	SAFE_RELEASE(mPD3D9DOBj);
	return S_OK;
}

DWORD D3D9Render::RenderLoop()
{
	while (mRenderThreadRuning){
		Sleep(10);
	}

	return 0;
}

HRESULT D3D9Render::PushFrame(CSampleBuffer *frame)
{
	HRESULT hr = E_FAIL;

	return hr;
}

BOOL D3D9Render::OSDText(HDC, TCHAR *format, ...)
{
	HRESULT hr = S_OK;
	RECT FontPos;
	TCHAR buf[1024] = { '\0' };
	va_list va_alist;

	GetWindowRect(mhWnd, &FontPos);

	va_start(va_alist, format);
	vswprintf_s(buf, format, va_alist);
	va_end(va_alist);

	CHECK_HR(hr = mPD3DDevice->BeginScene());
	hr = mPFont->DrawText(NULL, buf, -1, &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 255, 0));
	CHECK_HR(hr = mPD3DDevice->EndScene());

done:
	return hr == S_OK;
}