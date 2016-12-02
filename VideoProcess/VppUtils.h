#pragma once

#include "FFmpegWrapper.h"

AVPixelFormat GetPixFmtByFourCC(DWORD pfFourCC);
DWORD GetFourCCByPixFmt(AVPixelFormat format);
