#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "AutoLock.h"

#pragma comment(lib, "d3dx9.lib") //link the library
#pragma comment(lib, "d3d9.lib") //link the library

class D3D9Render
{
public:
	D3D9Render();
	D3D9Render(HWND hWnd);
	~D3D9Render();

	HRESULT InitializeRenderContext(int width, int height, DWORD pixelFormatInFourCC);
	HRESULT DeinitRenderContext();
	HRESULT PushFrame(CSampleBuffer *frame);
	DWORD RenderLoop();

protected:
	BOOL OSDText(HDC, TCHAR *, ...);
	void FourCCtoD3DFormat(D3DFORMAT *pd3dPixelFormat, DWORD dwFourCC);
	const TCHAR* GetErrorString(HRESULT hr);
	void SetupMatrices();
	HRESULT IfSupportedFormat(D3DFORMAT);

private:
	HWND mhWnd;
	IDirect3D9* mpD3D9OBj;
	IDirect3DDevice9* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	IDirect3DSurface9* mPrimerySurface;
	LPD3DXFONT mPFont;
	IDirect3DTexture9* mPrimeryTexture;

	HANDLE mRenderEvent;
	BOOL mSupportVSync;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;
};

