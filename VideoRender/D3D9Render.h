#pragma once

#include "IRender.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "AutoLock.h"

#pragma comment(lib, "d3dx9.lib") //link the library
#pragma comment(lib, "d3d9.lib") //link the library

class D3D9Render :public IRender
{
public:
	D3D9Render();
	~D3D9Render();

	BOOL InitRender(HWND , int , int , DWORD );
	BOOL DeinitRender();
	BOOL PushFrame(CSampleBuffer *);
	BOOL Repaint() { return S_OK; };

	const char *GetRenderDescriptor() { return "D39 Render"; };

	DWORD RenderLoop();

protected:
	BOOL OSDText(HDC, TCHAR *, ...);
	const TCHAR* GetErrorString(HRESULT hr);
	void SetupMatrices();
	HRESULT IfSupportedFormat(D3DDISPLAYMODE, D3DFORMAT, BOOL &);
	HRESULT GetDisplayMode();

private:
	HWND mhWnd;
	IDirect3D9* mpD3D9OBj;
	IDirect3DDevice9* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	IDirect3DSurface9* mPrimerySurface;
	IDirect3DSurface9* mpRenderTarget;
	LPD3DXFONT mPFont;
	IDirect3DTexture9* mPrimeryTexture[3]; /* for YUV */

	HANDLE mRenderEvent;
	BOOL mSupportVSync;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;
	BOOL mbNeedConversion;

	// VPP 
	IVPPFactory *mVppFactory;
	IVPP *mVpp;
	IVPPPARAMETER vppParams;
};

