
#pragma once

#ifdef VIDEOCODEC_EXPORTS
#define VIDEOCODEC_API __declspec(dllexport)
#else
#define VIDEOCODEC_API __declspec(dllimport)
#endif

