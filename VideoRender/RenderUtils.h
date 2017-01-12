#pragma once

D3DFORMAT GetD3D9PixelFmtByFourCC(DWORD pfFourCC);
DWORD GetFourCCByD3D9PixelFmt(D3DFORMAT format);
const TCHAR* GetD3D9ErrorString(HRESULT hr);
DWORD WINAPI CreateRenderThread(void* args);
const TCHAR* GetDDrawErrorString(HRESULT hr);

