#pragma once

#include <d3d9.h>
#include <d3dx9.h>

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

private:
	HWND mhWnd;
	IDirect3D9* mPD3D9DOBj;
	IDirect3DDevice9* mPD3DDevice;
	IDirect3DSurface9* mPrimerySurface;
	LPD3DXFONT mPFont;


	HANDLE mRenderEvent;
	BOOL mSupportVSync;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;
};

