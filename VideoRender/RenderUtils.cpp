#include "stdafx.h"
#include "RenderUtils.h"
#include <d3d9.h>

typedef struct tagPixelFormatD3DXAndFourCC
{
	DWORD pfFourCC;
	D3DFORMAT d3d9Format;
}PFDF;

PFDF PFDFTable[] = {
	{ PIXEL_FORMAT_RGB24, D3DFMT_R8G8B8 },
	{ PIXEL_FORMAT_RGB32, D3DFMT_A8R8G8B8 },
	{ PIXEL_FORMAT_ARGB, D3DFMT_A8R8G8B8 },
	{ PIXEL_FORMAT_RGB565, D3DFMT_R5G6B5 },
	{ PIXEL_FORMAT_RGB555, D3DFMT_X1R5G5B5 },
	{ PIXEL_FORMAT_UYVY, (D3DFORMAT)PIXEL_FORMAT_UYVY },
	{ PIXEL_FORMAT_YUY2, D3DFMT_YUY2 },
	{ PIXEL_FORMAT_UYVY, D3DFMT_UYVY },
	{ PIXEL_FORMAT_I420, (D3DFORMAT)PIXEL_FORMAT_I420 },
	{ PIXEL_FORMAT_NV12, (D3DFORMAT)PIXEL_FORMAT_NV12 },
	{ PIXEL_FORMAT_YV12, (D3DFORMAT)PIXEL_FORMAT_YV12 }
};

D3DFORMAT GetD3D9PixelFmtByFourCC(DWORD pfFourCC)
{
	D3DFORMAT fp = D3DFMT_UNKNOWN;
	for (int i = 0; i < ARRAYSIZE(PFDFTable); i++){
		if (PFDFTable[i].pfFourCC == pfFourCC){
			fp = PFDFTable[i].d3d9Format;
		}
	}
	return fp;
}

DWORD GetFourCCByD3D9PixelFmt(D3DFORMAT format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PFDFTable); i++){
		if (PFDFTable[i].d3d9Format == format){
			fourCC = PFDFTable[i].pfFourCC;
		}
	}
	return fourCC;
}