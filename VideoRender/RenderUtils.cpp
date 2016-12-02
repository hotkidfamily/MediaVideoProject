#include "stdafx.h"
#include "RenderUtils.h"
#include <d3d9.h>

typedef struct tagPixelFormatD3DXAndFourCC
{
	DWORD pfFourCC;
	D3DFORMAT d3d9Format;
}PFDF;

PFDF PFDFTable[] = {
	{ 0x32334354, D3DFMT_A8B8G8R8 },
	{ 0x32334355, D3DFMT_R5G6B5 },
};

D3DFORMAT TranslateFourCCToD3D9PixelFormat(DWORD pfFourCC)
{
	D3DFORMAT fp = D3DFMT_UNKNOWN;
	for (int i = 0; i < ARRAYSIZE(PFDFTable); i++){
		if (PFDFTable[i].pfFourCC == pfFourCC){
			fp = PFDFTable[i].d3d9Format;
		}
	}
	return fp;
}

DWORD TranslateD3D9PixelFormatToFourCC(D3DFORMAT format)
{
	DWORD fourCC = 0;
	for (int i = 0; i < ARRAYSIZE(PFDFTable); i++){
		if (PFDFTable[i].d3d9Format == format){
			fourCC = PFDFTable[i].pfFourCC;
		}
	}
	return fourCC;
}