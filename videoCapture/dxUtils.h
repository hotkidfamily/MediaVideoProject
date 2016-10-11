#pragma once

#include <dshow.h>
#include <dvdmedia.h>

#define UNUSED_FOURCC_SUPPORT {'ffff', TEXT("ffff"), VideoOutputType_None}
#define UNUSED_FOURCC_CODE (0xffff)

typedef enum tagVideoOutputFormat{
	VideoOutputType_None,
	VideoOutputType_RGB24,
	VideoOutputType_RGB32,
	VideoOutputType_ARGB32,

	VideoOutputType_I420,
	VideoOutputType_YV12,

	VideoOutputType_Y41P,
	VideoOutputType_YVU9,

	VideoOutputType_YVYU,
	VideoOutputType_YUY2,
	VideoOutputType_UYVY,
	VideoOutputType_HDYC,

	VideoOutputType_MPEG2_VIDEO,
	VideoOutputType_H264,

	VideoOutputType_dvsl,
	VideoOutputType_dvsd,
	VideoOutputType_dvhd,

	VideoOutputType_MJPG
}VIDEOFORMAT;

class dxUtils
{
public:
	dxUtils();
	~dxUtils();

	static BITMAPINFOHEADER *getBmpHeader(AM_MEDIA_TYPE *pmt);
	static double calcFps(REFERENCE_TIME rt);
	static REFERENCE_TIME *getFrameInterval(AM_MEDIA_TYPE *pmt);
	static TCHAR* getInfo(GUID id, DWORD fourCC);
	static void FreeMediaType(AM_MEDIA_TYPE& mt);
	static void DeleteMediaType(AM_MEDIA_TYPE *pmt);
	static VIDEOFORMAT getVideoType(AM_MEDIA_TYPE *mt);

private:
	static TCHAR* guid2str(GUID id);
	static TCHAR* fourCCStr(DWORD fourCC);
	static VIDEOFORMAT getVideoTypeByGuid(GUID id);
	static VIDEOFORMAT getVideoTypeByFourCC(DWORD fourCC);
};

