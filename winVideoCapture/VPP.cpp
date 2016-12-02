#include "stdafx.h"
#include "VPP.h"


CVPP::CVPP()
{
}


CVPP::~CVPP()
{
}

BOOL CVPP::convertYUY2toI420(x264_picture_t &dstPic, const CSampleBuffer *srcPic)
{
	// yuyv ==> y + u + v
	uint8_t *y = nullptr, *u = nullptr, *v = nullptr;
	uint8_t *yp = nullptr;
	int step = 0;

	int32_t width = srcPic->GetWidth();
	int32_t height = srcPic->GetHeight();
	int32_t linSize = srcPic->GetLineSize();

	y = dstPic.img.plane[0];
	yp = srcPic->GetDataPtr();
	for (int32_t i = 0; i < height*width; i++){
		y[i] = yp[i * 2];
	}

	u = dstPic.img.plane[1];
	v = dstPic.img.plane[2];
	yp = srcPic->GetDataPtr();
	for (int32_t j = 0; j < height; j += 2){
		yp = srcPic->GetDataPtr() + j*linSize;
		u = dstPic.img.plane[1] + j / 2 * dstPic.img.i_stride[1];
		v = dstPic.img.plane[2] + j / 2 * dstPic.img.i_stride[2];
		for (int32_t i = 0; i < width / 2; i++){
			u[i] = (yp[i * 4 + 1] + yp[i * 4 + linSize + 1]) / 2;
			v[i] = (yp[i * 4 + 3] + yp[i * 4 + linSize + 3]) / 2;
		}
	}

	return TRUE;
}

BOOL CVPP::convertYUY2toNV16(x264_picture_t &dstPic, const CSampleBuffer *srcPic)
{
	uint8_t *y = nullptr, *uv = nullptr;
	uint8_t *yp = nullptr;
	int step = 0;

	int32_t width = srcPic->GetWidth();
	int32_t height = srcPic->GetHeight();
	int32_t linSize = srcPic->GetLineSize();

	for (int32_t j = 0; j < height; j++){
		y = dstPic.img.plane[0] + j * dstPic.img.i_stride[0];
		uv = dstPic.img.plane[1] + j * dstPic.img.i_stride[1];
		yp = srcPic->GetDataPtr() + j * linSize;

		for (int32_t i = 0, k = 0; i < linSize; i += 4, k += 2){
			y[k] = yp[i];
			uv[k] = yp[i + 1];
			y[k + 1] = yp[i + 2];
			uv[k + 1] = yp[i + 3];
		}
	}

	return TRUE;
}
