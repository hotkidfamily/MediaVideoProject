#pragma once

#include "FFmpegWrapper.h"

AVPixelFormat TranslateFourCCToFFmpegPixelFormat(DWORD pfFourCC);
DWORD TranslateFFmpegPixelFormatToFourCC(AVPixelFormat format);
