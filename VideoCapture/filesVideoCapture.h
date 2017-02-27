#pragma once


#ifdef __cplusplus
extern "C" {
#endif

	#include "..\ffmpeg2.7.1\include\libavformat\avio.h"
	#include "..\ffmpeg2.7.1\include\libavformat\avformat.h"
	#include "..\ffmpeg2.7.1\include\libavcodec\avcodec.h"
	#include "..\ffmpeg2.7.1\include\libavutil\avutil.h"
	#include "..\ffmpeg2.7.1\include\libavutil\imgutils.h"

#ifdef __cplusplus
}
#endif


class FilesVideoCapture
{
public:
	FilesVideoCapture();
	~FilesVideoCapture();

	int32_t DecodeLoop();

protected:
	BOOL initVideoContext(const char *filename);
	int32_t decodePacket(int *got_frame, int cached, AVPacket &);
	void cleanUp();


private:
	AVFormatContext *mFileCtx;
	AVCodecContext *mVideoDecodeCtx;
	AVFrame* mDecDestFrame;
	uint8_t* mDecDestCopiedBuffer;
	int32_t mDecDestCopiedBufferSize;
	int64_t mFrameIndex;



	HANDLE mDecodeThreadHandle;
	DWORD mDecodeThreadID;
};