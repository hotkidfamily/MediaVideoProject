#pragma once

#include "PixelFormat.h"
#include <d3d9.h>

D3DFORMAT TranslateFourCCToD3D9PixelFormat(DWORD pfFourCC);
DWORD TranslateD3D9PixelFormatToFourCC(D3DFORMAT format);
