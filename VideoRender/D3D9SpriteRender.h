#pragma once

#include "IRender.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "SlidingWindowCalc.h"

#define MAX_RENDER_OBJ (10)

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
	HRESULT UpdateRenderSurface(CSampleBuffer *&);
	BOOL OSDText(HDC, RECT *, TCHAR *format, ...);
	BOOL RenderStatus();
	BOOL UpdatePushStatis(CSampleBuffer *&);
	BOOL UpdateRenderStatis();

private:
	HWND mhWnd;

	IDirect3D9Ex* mpD3D9OBj;
	IDirect3DDevice9Ex* mpD3D9Device;
	D3DDEVTYPE mD3D9DeviceType;
	D3DCAPS9 mpD3D9DeviceCaps;
	BOOL mbSupportConversion;

	enum SSurfaceType {
		SUPPORT_TEXTURE,
		SUPPORT_SURFACE
	};

	SSurfaceType mSupportSurfaceType;
	int32_t mCurPushObjIndex;
	int32_t mCurRenderObjIndex;
	IDirect3DTexture9 *mpD3D9Texture[MAX_RENDER_OBJ];
	IDirect3DSurface9 *mpD3D9Surface[MAX_RENDER_OBJ];

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
	CSampleBuffer *mVppTransSampleBuffer;

	// statistics 
	int64_t mLastPts;
	DWORD mLastRender;
	int32_t mCurRenderInterval;
	int64_t mCurPtsInterval;
	CSlidingWindowCalc mInputStatis;
	CSlidingWindowCalc mRenderStatis;
};

