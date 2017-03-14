#include "stdafx.h"
#include "RenderUtils.h"
#include "iRenderCommon.h"

typedef struct tagPixelFormatD3DXAndFourCC
{
	DWORD pfFourCC;
	D3DFORMAT d3d9Format;
	D3DFORMAT prefd3d9Format;
	TCHAR *desc;
}PFDF;

PFDF PixelAndD3d9FmtTable[] = {
	{ PIXEL_FORMAT_RGB24, D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, _T("R8G8B8") },
	{ PIXEL_FORMAT_RGB32, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, _T("X8R8G8B8") },
	{ PIXEL_FORMAT_ARGB, D3DFMT_A8R8G8B8, D3DFMT_A8R8G8B8, _T("A8R8G8B8") },
	{ PIXEL_FORMAT_RGB565, D3DFMT_R5G6B5, D3DFMT_R5G6B5, _T("R5G6B5") },
	{ PIXEL_FORMAT_RGB555, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, _T("X1R5G5B5") },
	{ PIXEL_FORMAT_YUY2, D3DFMT_YUY2, (D3DFORMAT)PIXEL_FORMAT_YV12, _T("YUY2") },
	{ PIXEL_FORMAT_UYVY, D3DFMT_UYVY, (D3DFORMAT)PIXEL_FORMAT_NV12, _T("UYVY") },
	{ PIXEL_FORMAT_I420, (D3DFORMAT)PIXEL_FORMAT_I420, (D3DFORMAT)PIXEL_FORMAT_YV12, _T("I420") },
	{ PIXEL_FORMAT_NV12, (D3DFORMAT)PIXEL_FORMAT_NV12, (D3DFORMAT)PIXEL_FORMAT_YV12, _T("NV12") },
	{ PIXEL_FORMAT_YV12, (D3DFORMAT)PIXEL_FORMAT_YV12, (D3DFORMAT)PIXEL_FORMAT_NV12, _T("YV12") }
};

D3DFORMAT GetPrefD3D9PixelFmtByFourCC(DWORD pfFourCC)
{
	D3DFORMAT fp = D3DFMT_UNKNOWN;
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].pfFourCC == pfFourCC){
			fp = PixelAndD3d9FmtTable[i].prefd3d9Format;
			break;
		}
	}
	return fp;
}

D3DFORMAT GetD3D9PixelFmtByFourCC(DWORD pfFourCC)
{
	D3DFORMAT fp = D3DFMT_UNKNOWN;
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].pfFourCC == pfFourCC){
			fp = PixelAndD3d9FmtTable[i].d3d9Format;
			break;
		}
	}
	return fp;
}

TCHAR *GetD3D9PixelFmtDescByFourCC(DWORD pfFourCC)
{
	TCHAR *desc = _T("unknow");
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].pfFourCC == pfFourCC){
			desc = PixelAndD3d9FmtTable[i].desc;
			break;
		}
	}
	return desc;
}

TCHAR *GetD3D9PixelFmtDescByD3D9Fmt(D3DFORMAT format)
{
	TCHAR *desc = _T("unknow");

	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].d3d9Format == format){
			desc = PixelAndD3d9FmtTable[i].desc;
			break;
		}
	}
	return desc;
}

DWORD GetFourCCByD3D9PixelFmt(D3DFORMAT format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PixelAndD3d9FmtTable); i++){
		if (PixelAndD3d9FmtTable[i].d3d9Format == format){
			fourCC = PixelAndD3d9FmtTable[i].pfFourCC;
			break;
		}
	}
	return fourCC;
}

DWORD WINAPI CreateRenderThread(void* args)
{
	IRenderThread *pRender = (IRenderThread*)args;
	return pRender->RenderLoop();
}