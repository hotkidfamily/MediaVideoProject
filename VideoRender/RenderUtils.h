#pragma once

D3DFORMAT GetD3D9PixelFmtByFourCC(DWORD pfFourCC);
DWORD GetFourCCByD3D9PixelFmt(D3DFORMAT format);
DWORD WINAPI CreateRenderThread(void* args);

