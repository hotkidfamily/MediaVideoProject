#ifndef __FFMPEGWRAPPER_H__
#define __FFMPEGWRAPPER_H__

#include "wrapperCommon.h"
#include <stdint.h>

extern "C" {
#include "libswscale\swscale.h"
}

namespace FFmpegWrapper
{
	FUNCTION_DECLARE(const int *, sws_getCoefficients, (int colorspace));
	FUNCTION_DECLARE(int, sws_isSupportedInput, (enum AVPixelFormat pix_fmt));
	FUNCTION_DECLARE(int, sws_isSupportedOutput, (enum AVPixelFormat pix_fmt));
	FUNCTION_DECLARE(int, sws_isSupportedEndiannessConversion, (enum AVPixelFormat pix_fmt));
	FUNCTION_DECLARE(struct SwsContext *, sws_alloc_context, (void));
	FUNCTION_DECLARE(int, sws_init_context, (struct SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter));
	FUNCTION_DECLARE(void, sws_freeContext, (struct SwsContext *swsContext));
	FUNCTION_DECLARE(struct SwsContext *, sws_getContext, (int srcW, int srcH, enum AVPixelFormat srcFormat,
					int dstW, int dstH, enum AVPixelFormat dstFormat, int flags, SwsFilter *srcFilter,
						SwsFilter *dstFilter, const double *param));
	FUNCTION_DECLARE(int, sws_scale, (struct SwsContext *c, const uint8_t *const srcSlice[],
									const int srcStride[], int srcSliceY, int srcSliceH,
									uint8_t *const dst[], const int dstStride[]));
	FUNCTION_DECLARE(int, sws_setColorspaceDetails, (struct SwsContext *c, const int inv_table[4], 
					int srcRange, const int table[4], int dstRange,
					int brightness, int contrast, int saturation));
	FUNCTION_DECLARE(int, sws_getColorspaceDetails, (struct SwsContext *c, int **inv_table, 
					int *srcRange, int **table, int *dstRange,
					int *brightness, int *contrast, int *saturation));
	FUNCTION_DECLARE(SwsVector *, sws_allocVec, (int length));
	FUNCTION_DECLARE(SwsVector *, sws_getGaussianVec, (double variance, double quality));
	FUNCTION_DECLARE(SwsVector *, sws_getConstVec, (double c, int length));
	FUNCTION_DECLARE(SwsVector *, sws_getIdentityVec, (void));
	FUNCTION_DECLARE(void, sws_scaleVec, (SwsVector *a, double scalar));
	FUNCTION_DECLARE(void, sws_normalizeVec, (SwsVector *a, double height));
	FUNCTION_DECLARE(void, sws_convVec, (SwsVector *a, SwsVector *b));
	FUNCTION_DECLARE(void, sws_addVec, (SwsVector *a, SwsVector *b));
	FUNCTION_DECLARE(void, sws_subVec, (SwsVector *a, SwsVector *b));
	FUNCTION_DECLARE(void, sws_shiftVec, (SwsVector *a, int shift));
	FUNCTION_DECLARE(SwsVector *, sws_cloneVec, (SwsVector *a));
	FUNCTION_DECLARE(void, sws_printVec2, (SwsVector *a, AVClass *log_ctx, int log_level));
	FUNCTION_DECLARE(void, sws_freeVec, (SwsVector *a));
	FUNCTION_DECLARE(SwsFilter *, sws_getDefaultFilter, (float lumaGBlur, float chromaGBlur,
					float lumaSharpen, float chromaSharpen, float chromaHShift, float chromaVShift, int verbose));
	FUNCTION_DECLARE(void, sws_freeFilter, (SwsFilter *filter));
	FUNCTION_DECLARE(struct SwsContext *, sws_getCachedContext, (struct SwsContext *context, 
		int srcW, int srcH, enum AVPixelFormat srcFormat, int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter *srcFilter,	SwsFilter *dstFilter, const double *param));
	FUNCTION_DECLARE(void, sws_convertPalette8ToPacked32, (const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette));
	FUNCTION_DECLARE(void, sws_convertPalette8ToPacked24, (const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette));
	FUNCTION_DECLARE(const AVClass *, sws_get_class, (void));

	bool InitModules();
    void UnInitModules();
}

#endif // __FFMPEGWRAPPER_H__