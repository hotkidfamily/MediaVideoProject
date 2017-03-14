#pragma once

#include "AudioSampleBufferManager.h"
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
	virtual BOOL GetFrame(VideoSampleBuffer *&pSample);
	virtual BOOL ReleaseFrame(VideoSampleBuffer *&pSample);

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
	BOOL initFileParse(const char *filename);
	BOOL initVideoCodec();
	BOOL initAudioCodec();
	int32_t decodeVideoPacket(int *got_frame, AVPacket &);
	int32_t decodeAudioPacket(int *got_frame, AVPacket &);
	void cleanUp();


private:
	AVFormatContext *mFileCtx;

	AVCodecContext *mVideoDecodeCtx;
	AVFrame* mVideoDecDestFrame;
	AVStream *mVideoStream;
	int32_t mVideoStreamIndex;
	uint8_t* mDecDestCopiedBuffer;
	int32_t mDecDestCopiedBufferSize;
	int64_t mFrameIndex;
	RATE_DESC mVideoFrameRate;
	int64_t mVideStreamPtsStep;

	AVFrame* mAudioDecDestFrame;
	AVStream *mAudioStream;
	AVCodecContext *mAudioDecodeCtx;
	int32_t mAudioStreamIndex;

	HANDLE mDecodeThreadHandle;
	DWORD mDecodeThreadID;
	BOOL mDecodeThreadQuit;
	int64_t mLastVideoFramePts;
	BOOL mbDecodeLoop;

	VideoSampleBufferManager mVideoSampleBufferManager;
	AudioSampleBufferManager mAudioSampleBufferManager;
	CClock *mBaseClock;
};
