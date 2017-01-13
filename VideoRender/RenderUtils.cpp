#include "stdafx.h"
#include "RenderUtils.h"
#include "iRenderCommon.h"
#include <d3d9.h>
#include <ddraw.h>

typedef struct tagPixelFormatD3DXAndFourCC
{
	DWORD pfFourCC;
	D3DFORMAT d3d9Format;
}PFDF;

PFDF PixelAndD3d9FmtTable[] = {
		{ PIXEL_FORMAT_RGB24, D3DFMT_R8G8B8 },
		{ PIXEL_FORMAT_RGB32, D3DFMT_X8R8G8B8 },
		{ PIXEL_FORMAT_ARGB, D3DFMT_A8R8G8B8 },
		{ PIXEL_FORMAT_RGB565, D3DFMT_R5G6B5 },
		{ PIXEL_FORMAT_RGB555, D3DFMT_X1R5G5B5 },
		{ PIXEL_FORMAT_YUY2, D3DFMT_YUY2 },
		{ PIXEL_FORMAT_UYVY, D3DFMT_UYVY },
		{ PIXEL_FORMAT_I420, (D3DFORMAT)PIXEL_FORMAT_I420 },
		{ PIXEL_FORMAT_NV12, (D3DFORMAT)PIXEL_FORMAT_NV12 },
		{ PIXEL_FORMAT_YV12, (D3DFORMAT)PIXEL_FORMAT_YV12 }
};

D3DFORMAT GetD3D9PixelFmtByFourCC(DWORD pfFourCC)
{
	D3DFORMAT fp = D3DFMT_UNKNOWN;
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].pfFourCC == pfFourCC){
			fp = PixelAndD3d9FmtTable[i].d3d9Format;
		}
	}
	return fp;
}

DWORD GetFourCCByD3D9PixelFmt(D3DFORMAT format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].d3d9Format == format){
			fourCC = PixelAndD3d9FmtTable[i].pfFourCC;
		}
	}
	return fourCC;
}

DWORD WINAPI CreateRenderThread(void* args)
{
	IRenderThread *pRender = (IRenderThread*)args;
	return pRender->RenderLoop();
}