#include "stdafx.h"
#include "FFmpegWrapper.h"

namespace FFmpegWrapper{
	const TCHAR * const defaultDllName = TEXT("swscale-3.dll");
	static DwWrapperCommon* pFFmpegWrapper = NULL;

	bool InitModules()
	{
		if (pFFmpegWrapper)
			return true;

		pFFmpegWrapper = new DwWrapperCommon(defaultDllName);

		return pFFmpegWrapper != NULL;
	}

	void UnInitModules()
	{
		if (pFFmpegWrapper) {
			delete pFFmpegWrapper;
			pFFmpegWrapper = NULL;
		}
	}

	int sws_isSupportedInput(enum AVPixelFormat pix_fmt)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_isSupportedInput func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_isSupportedInput");
		if (bRet) {
			ret = func(pix_fmt);
		}

		return ret;
	}

	int sws_isSupportedOutput(enum AVPixelFormat pix_fmt)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_isSupportedOutput func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_isSupportedOutput");
		if (bRet) {
			ret = func(pix_fmt);
		}
		return ret;
	}

	int sws_isSupportedEndiannessConversion(enum AVPixelFormat pix_fmt)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_isSupportedEndiannessConversion func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_isSupportedEndiannessConversion");
		if (bRet) {
			ret = func(pix_fmt);
		}
		return ret;
	}

	struct SwsContext * sws_alloc_context(void)
	{
		struct SwsContext * ctx = NULL;
		BOOL bRet = FALSE;  
		Psws_alloc_context func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_alloc_context");
		if (bRet) {
			ctx = func();
		}
		return ctx;
	}

	int sws_init_context(struct SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_init_context func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_init_context");
		if (bRet) {
			ret = func(sws_context, srcFilter, dstFilter);
		}
		return ret;
	}

	void sws_freeContext(struct SwsContext *swsContext)
	{
		BOOL bRet = FALSE;
		Psws_freeContext func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC*)func, "sws_freeContext");
		if (bRet) {
			func(swsContext);
		}
	}

	struct SwsContext * sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter *srcFilter,
		SwsFilter *dstFilter, const double *param)
	{
		struct SwsContext * ctx = NULL;
		BOOL bRet = FALSE;
		Psws_getContext func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getContext");
		if (bRet) {
			ctx = func(srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, srcFilter, dstFilter, param);
		}
		return ctx;
	}

	int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t *const dst[], const int dstStride[])
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_scale func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_scale");
		if (bRet) {
			ret = func(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
		}
		return ret;
	}

	int sws_setColorspaceDetails(struct SwsContext *c, const int inv_table[4],
		int srcRange, const int table[4], int dstRange,
		int brightness, int contrast, int saturation)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_setColorspaceDetails func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_setColorspaceDetails");
		if (bRet) {
			ret = func(c, inv_table, srcRange, table, dstRange, brightness, contrast, saturation);
		}
		return ret;
	}

	int sws_getColorspaceDetails(struct SwsContext *c, int **inv_table,
		int *srcRange, int **table, int *dstRange,
		int *brightness, int *contrast, int *saturation)
	{
		int ret = -1;
		BOOL bRet = FALSE;
		Psws_getColorspaceDetails func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getColorspaceDetails");
		if (bRet) {
			ret = func(c, inv_table, srcRange, table, dstRange, brightness, contrast, saturation);
		}
		return ret;
	}

	SwsVector * sws_allocVec(int length)
	{
		SwsVector * vect = NULL;
		BOOL bRet = FALSE;
		Psws_allocVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_allocVec");
		if (bRet) {
			vect = func(length);
		}
		return vect;
	}

	SwsVector * sws_getGaussianVec(double variance, double quality)
	{
		SwsVector * vect = NULL;
		BOOL bRet = FALSE;
		Psws_getGaussianVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getGaussianVec");
		if (bRet) {
			vect = func(variance, quality);
		}
		return vect;
	}

	SwsVector * sws_getConstVec(double c, int length)
	{
		SwsVector * vect = NULL;
		BOOL bRet = FALSE;
		Psws_getConstVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getConstVec");
		if (bRet) {
			vect = func(c, length);
		}
		return vect;
	}

	SwsVector * sws_getIdentityVec(void)
	{
		SwsVector * vect = NULL;
		BOOL bRet = FALSE;
		Psws_getIdentityVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getIdentityVec");
		if (bRet) {
			vect = func();
		}
		return vect;
	}

	void sws_scaleVec(SwsVector *a, double scalar)
	{
		BOOL bRet = FALSE;
		Psws_scaleVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC*)func, "sws_scaleVec");
		if (bRet){
			func(a, scalar);
		}
	}

	void sws_normalizeVec(SwsVector *a, double height)
	{
		BOOL bRet = FALSE;
		Psws_normalizeVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_normalizeVec");
		if (bRet){
			func(a, height);
		}
	}

	void sws_convVec(SwsVector *a, SwsVector *b)
	{
		BOOL bRet = FALSE;
		Psws_convVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_convVec");
		if (bRet){
			func(a, b);
		}
	}

	void sws_addVec(SwsVector *a, SwsVector *b)
	{
		BOOL bRet = FALSE;
		Psws_addVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_addVec");
		if (bRet){
			func(a, b);
		}
	}

	void sws_subVec(SwsVector *a, SwsVector *b)
	{
		BOOL bRet = FALSE;
		Psws_subVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_subVec");
		if (bRet){
			func(a, b);
		}
	}

	void sws_shiftVec(SwsVector *a, int shift)
	{
		BOOL bRet = FALSE;
		Psws_shiftVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_shiftVec");
		if (bRet){
			func(a, shift);
		}
	}

	SwsVector * sws_cloneVec(SwsVector *a)
	{
		SwsVector * vect = NULL;
		BOOL bRet = FALSE;
		Psws_cloneVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_cloneVec");
		if (bRet) {
			func(a);
		}
		return vect;
	}

	void sws_printVec2(SwsVector *a, AVClass *log_ctx, int log_level)
	{
		BOOL bRet = FALSE;
		Psws_printVec2 func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_printVec2");
		if (bRet){
			func(a, log_ctx, log_level);
		}
	}

	void sws_freeVec(SwsVector *a)
	{
		BOOL bRet = FALSE;
		Psws_freeVec func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_freeVec");
		if (bRet){
			func(a);
		}
	}

	SwsFilter * sws_getDefaultFilter(float lumaGBlur, float chromaGBlur,
		float lumaSharpen, float chromaSharpen,
		float chromaHShift, float chromaVShift,
		int verbose)
	{
		struct SwsFilter * filter = NULL;
		BOOL bRet = FALSE;
		Psws_getDefaultFilter func = NULL;

		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getDefaultFilter");
		if (bRet) {
			filter = func(lumaGBlur, chromaGBlur, lumaSharpen, chromaSharpen, chromaHShift, chromaVShift, verbose);
		}
		return filter;
	}

	void sws_freeFilter(SwsFilter *filter)
	{
		BOOL bRet = FALSE;
		Psws_freeFilter func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_freeFilter");
		if (bRet){
			func(filter);
		}
	}

	struct SwsContext * sws_getCachedContext(struct SwsContext *context,
		int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter *srcFilter,
		SwsFilter *dstFilter, const double *param)
	{
		struct SwsContext * ctx = NULL;

		BOOL bRet = FALSE;
		Psws_getCachedContext func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_getDefaultFilter");
		if (bRet) {
			ctx = func(context, srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, srcFilter, dstFilter, param);
		}
		return ctx;
	}

	void sws_convertPalette8ToPacked32(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette)
	{
		BOOL bRet = FALSE;
		Psws_convertPalette8ToPacked32 func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_convertPalette8ToPacked32");
		if (bRet){
			func(src, dst, num_pixels, palette);
		}
	}

	void sws_convertPalette8ToPacked24(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette)
	{
		BOOL bRet = FALSE;
		Psws_convertPalette8ToPacked24 func = NULL;
		bRet = pFFmpegWrapper->DwGetProcAddr((FARPROC *)func, "sws_convertPalette8ToPacked24");
		if (bRet){
			func(src, dst, num_pixels, palette);
		}
	}
}