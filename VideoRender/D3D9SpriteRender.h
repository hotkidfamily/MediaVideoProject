#pragma once

#include "IRender.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "SlidingWindowCalc.h"

class D3D9SpriteRender : public IRender
{
public:
	D3D9SpriteRender();
	~D3D9SpriteRender();

public:
	virtual BOOL Repaint() { return TRUE; }
	virtual BOOL InitRender(HWND, int32_t, int32_t, DWORD);
	virtual BOOL DeinitRender();

	virtual BOOL PushFrame(CSampleBuffer *);
	virtual const char *GetRenderDescriptor() const { return "D3D9 sprite render"; };
	DWORD RenderLoop();


protected:
	void SetupMatrices();
	HRESULT GetDeviceType(D3DDISPLAYMODE);
	HRESULT IfSupportedFormat(D3DDISPLAYMODE , D3DFORMAT );
	BOOL IfSupportedConversionFormat(D3DDISPLAYMODE , D3DFORMAT);
	HRESULT updateContent(CSampleBuffer *&);
	BOOL OSDText(HDC, TCHAR *format, ...);
	BOOL OutputInformation();

private:
	HWND mhWnd;
	IDirect3D9* mpD3D9OBj;
	IDirect3DDevice9* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	BOOL mbSupportConversion;
	enum surfaceType {
		SUPPORT_TEXTURE,
		SUPPORT_SURFACE
	};
	int32_t mSupportSurfaceType;
	IDirect3DTexture9 *mpD3D9Texture;
	IDirect3DTexture9 *mpD3D9Texture2;
	IDirect3DSurface9 *mpD3D9Surface;
	IDirect3DSurface9 *mpD3D9Surface2;
	volatile LPVOID mpReadyObj;
	volatile LPVOID mpFreeObj;

	CRITICAL_SECTION cs;

	LPD3DXFONT mPFont;
	ID3DXSprite *mSprite;

	HANDLE mRenderEvent;
	HANDLE mRenderThreadHandle;
	DWORD mRenderThreadId;
	BOOL mRenderThreadRuning;

	// VPP 
	BOOL mbNeedVpp;
	IVPPFactory *mVppFactory;
	IVPP *mVpp;
	IVPPPARAMETER vppParams;
	CSampleBuffer *transSampleBuffer;

	// statistics 
	int64_t mLastPts;
	int64_t mLastRender;
	CSlidingWindowCalc mInputStatis;
	CSlidingWindowCalc mRenderStatis;
};

