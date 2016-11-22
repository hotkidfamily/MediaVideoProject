#pragma once
extern "C" {
#include <stdint.h>
#include "x264.h"
}

class CVPP
{
public:
	CVPP();
	~CVPP();

	BOOL convertYUY2toI420(x264_picture_t &dstPic, const CSampleBuffer *srcPic);
	BOOL convertYUY2toNV16(x264_picture_t &dstPic, const CSampleBuffer *srcPic);
};

