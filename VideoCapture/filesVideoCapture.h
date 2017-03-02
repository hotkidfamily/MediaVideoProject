#pragma once

#include "SampleBufferManager.h"
#include "IVideoCapture.h"

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


class FilesVideoCapture: public IVideoCapture
{
public:
	FilesVideoCapture(CClock &);
	~FilesVideoCapture();

	int32_t DecodeLoop();
	virtual BOOL GetFrame(CSampleBuffer *&pSample);
	virtual BOOL ReleaseFrame(CSampleBuffer *&pSample);

	virtual void RegisterCallback(VideoCaptureCallback *) { return; };
	// step 1, get device list
	virtual HRESULT GetDeviceList(VECT &) { return E_NOTIMPL; };
	// step 2, start capture
	virtual HRESULT StartCaptureWithParam(CAPTURECONFIG&);
	
	// step 3.x other feature support: show property setting window
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) { return E_NOTIMPL; };

	// step 4, close capture
	virtual HRESULT StopCapture();
	// step 5, release callback
	virtual HRESULT UnRegisterCallback() { return E_NOTIMPL; };

protected:
	BOOL initVideoContext(const char *filename);
	int32_t decodePacket(int *got_frame, AVPacket &);
	void cleanUp();


private:
	AVFormatContext *mFileCtx;
	AVCodecContext *mVideoDecodeCtx;
	AVFrame* mDecDestFrame;
	AVStream *mVideoStream;
	uint8_t* mDecDestCopiedBuffer;
	int32_t mDecDestCopiedBufferSize;
	int64_t mFrameIndex;
	int32_t mVideoStreamIndex;

	double mFrameRate;
	int64_t mVideStreamPtsStep;

	HANDLE mDecodeThreadHandle;
	DWORD mDecodeThreadID;
	BOOL mDecodeThreadQuit;

	CSampleBufferManager mBufferManager;
	CClock *mBaseClock;
};
