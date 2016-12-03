#pragma once

#include "IRender.h"
#include <d3d9.h>
#include <d3dx9.h>

class D3D9SpriteRender : public IRender
{
public:
	D3D9SpriteRender();
	~D3D9SpriteRender();

public:
	virtual BOOL Repaint() { return TRUE; }
	virtual BOOL InitRender(HWND, int32_t, int32_t, DWORD);
	virtual BOOL DeinitRender();

	virtual BOOL PushFrame(CSampleBuffer *frame);
	virtual const char *GetRenderDescriptor() const { return "D3D9 sprite render"; };
	DWORD RenderLoop();
	BOOL OSDText(HDC, TCHAR *format, ...);


protected:
	void SetupMatrices();
	HRESULT GetDeviceType(D3DDISPLAYMODE);
	HRESULT IfSupportedFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat);
	BOOL IfSupportedConversionFormat(D3DDISPLAYMODE mode, D3DFORMAT pixelFormat);


private:
	HWND mhWnd;
	IDirect3D9* mpD3D9OBj;
	IDirect3DDevice9* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	IDirect3DTexture9 *mpD3D9Texture;
	IDirect3DTexture9 *mpD3D9Texture2;
	volatile LPDIRECT3DTEXTURE9 mpReadyTexture;
	volatile LPDIRECT3DTEXTURE9 mpFreeTexture;

	LPD3DXFONT mPFont;
	ID3DXSprite *mSprite;
	D3DXMATRIX matrixWorld;

	HANDLE mRenderEvent;
	BOOL mSupportVSync;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;

	// VPP 
	IVPPFactory *mVppFactory;
	IVPP *mVpp;
	IVPPPARAMETER vppParams;
};

