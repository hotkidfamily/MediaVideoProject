#include "stdafx.h"
#include "filesVideoCapture.h"

DWORD WINAPI decodeThread(void *args)
{
	FilesVideoCapture *pCapture = (FilesVideoCapture *)args;
	return pCapture->DecodeLoop();
}

BOOL FilesVideoCapture::initVideoContext(const char *filename)
{
	AVStream *vs = NULL;
	AVCodec *videoDec = NULL;
	BOOL bRet = FALSE;
	int ret = -1;
	AVRational time_base;

	mFrameIndex = 0;

	if (avformat_open_input(&mFileCtx, filename, NULL, NULL) < 0) {
		logger(Error, "Could not open source file %s\n", filename);
		goto fail;
	}

	if (avformat_find_stream_info(mFileCtx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		goto fail;
	}

	ret = av_find_best_stream(mFileCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (ret < 0){
		logger(Error, "could not find video stream.\n");
		goto fail;
	}

	vs = mFileCtx->streams[ret];
	mVideoStreamIndex = ret;

	mVideoDecodeCtx = vs->codec;
	videoDec = avcodec_find_decoder(vs->codec->codec_id);

	/* Init the decoders, with or without reference counting */
	if ((ret = avcodec_open2(mVideoDecodeCtx, videoDec, NULL)) < 0) {
		logger(Error, "Failed to open %s codec\n");
		goto fail;
	}

	mBufferManager.Reset(RES1080P, 20);

	mDecDestFrame = av_frame_alloc();
	if (!mDecDestFrame) {
		logger(Error, "Could not allocate AVFrame.\n");
		goto fail;
	}

	mDecodeThreadQuit = FALSE;
	mDecodeThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)decodeThread, this, 0, &mDecodeThreadID);
	if (mDecodeThreadHandle == NULL){
		logger(Error, "Failed to create thread to decode file.\n");
		goto fail;
	}

	time_base = av_codec_get_pkt_timebase(mVideoDecodeCtx);
	time_base_step = time_base.num / time_base.den;

	bRet = TRUE;

fail:
	if (!bRet){
		cleanUp();
	}

	return bRet;
}

void FilesVideoCapture::cleanUp()
{
	if (mDecodeThreadHandle){
		mDecodeThreadQuit = TRUE;
		if (WAIT_OBJECT_0 != WaitForSingleObject(mDecodeThreadHandle, 1000)) {
			TerminateThread(mDecodeThreadHandle, -1);
			logger(Error, "Can not stop decode thread running.\n");
		}
		mDecodeThreadHandle = NULL;
	}
	
	if (mVideoDecodeCtx){
		avcodec_close(mVideoDecodeCtx);
		mVideoDecodeCtx = NULL;
	}

	if (mFileCtx){
		avformat_close_input(&mFileCtx);
		mFileCtx = NULL;
	}

	if (mDecDestCopiedBuffer) {
		DeallocMemory(mDecDestCopiedBuffer);
		mDecDestCopiedBuffer = NULL;
		mDecDestCopiedBufferSize = 0;
	}
	
	if (mDecDestFrame){
		av_frame_free(&mDecDestFrame);
		mDecDestFrame = NULL;
	}
}

int32_t FilesVideoCapture::decodePacket(int *got_frame, AVPacket &pkt)
{
	int ret = 0;
	int decoded = pkt.size;
	QUEUE_RET q_ret = Q_SUCCESS;

	*got_frame = 0;

	/* decode video frame */
	ret = avcodec_decode_video2(mVideoDecodeCtx, mDecDestFrame, got_frame, &pkt);
	if (ret < 0) {
		logger(Error, "decode video error.\n");
		goto fail;
	}

	if (*got_frame) {
		FRAME_DESC desc;

		desc.width = mDecDestFrame->width;
		desc.height = mDecDestFrame->height;
		desc.pixelFormatInFourCC = GetFourCCByPixFmt(mDecDestFrame->format);
		desc.ptsStart = mBaseClock->GetBaseTime() + av_frame_get_best_effort_timestamp(mDecDestFrame)*10000;
		desc.ptsEnd = desc.ptsStart + av_frame_get_pkt_duration(mDecDestFrame);
		//logger(Info, "pts %lld dts %lld, best %lld\n", mDecDestFrame->pkt_pts, mDecDestFrame->pkt_dts, desc.ptsStart);
		desc.frameStartIdx = mFrameIndex++;
		desc.frameEndIdx = mFrameIndex;

		desc.colorRange = (ColorRange)av_frame_get_color_range(mDecDestFrame);
		desc.colorPrimaries = (ColorPrimaries)mDecDestFrame->color_primaries;
		desc.colorTransfer = (ColorTransfer)mDecDestFrame->color_trc;

		int buf_len = av_image_get_buffer_size((AVPixelFormat)mDecDestFrame->format, mDecDestFrame->width, mDecDestFrame->height, 32);
		if (mDecDestCopiedBufferSize < buf_len){
			if(ReallocMemory(&mDecDestCopiedBuffer, buf_len))
				mDecDestCopiedBufferSize = buf_len;
		}

		ret = av_image_copy_to_buffer(mDecDestCopiedBuffer, buf_len, mDecDestFrame->data, 
			mDecDestFrame->linesize, (AVPixelFormat)mDecDestFrame->format, mDecDestFrame->width, mDecDestFrame->height, 32);

		if (ret > 0){
			desc.dataPtr = mDecDestCopiedBuffer;
			desc.validDataSize = mDecDestCopiedBufferSize;
		}
		// push frame to queue
		while ((q_ret = mBufferManager.FillFrame(desc)) != Q_SUCCESS){
			if (mDecodeThreadQuit){
				logger(Error, "End decode for exit thread.\n");
				break;
			}

			if (q_ret == Q_FULL){
				Sleep(2);
				continue;
			} else {
				logger(Error, "Can not push frame as: Queue is %d\n", q_ret);
				break;
			}
		}
	}

fail:
	return decoded;
}

int32_t FilesVideoCapture::DecodeLoop()
{
	int32_t ret = 0;
	AVPacket pkt;
	int32_t got_frame = 0;

	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	while (!mDecodeThreadQuit){
		if (mDecodeThreadQuit){
			break;
		}

		if (av_read_frame(mFileCtx, &pkt) >=0 ){
			if (pkt.stream_index != mVideoStreamIndex){
				continue;
			}
			AVPacket orig_pkt = pkt;
			do {
				if (mDecodeThreadQuit){
					logger(Error, "thread end by exit.\n");
					break;
				}
				ret = decodePacket(&got_frame, pkt);
				if (ret < 0)
					break;
				pkt.data += ret;
				pkt.size -= ret;
			} while (pkt.size > 0);
			av_packet_unref(&orig_pkt);
		}
	}

	/*
		remember flush decoder.
	*/

	return 0;
}

FilesVideoCapture::FilesVideoCapture(CClock &clock)
	: mDecodeThreadHandle(nullptr)
	, mDecodeThreadID(0)
	, mDecodeThreadQuit(0)
	, mFileCtx(NULL)
	, mDecDestCopiedBuffer(NULL)
	, mDecDestCopiedBufferSize(0)
	, mDecDestFrame(NULL)
	, mFrameIndex(0)
	, mVideoDecodeCtx(NULL)
	, mVideoStreamIndex(0)
	, mBaseClock(&clock)
{
	av_register_all();
}

FilesVideoCapture::~FilesVideoCapture()
{
	cleanUp();
}

BOOL FilesVideoCapture::GetFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mBufferManager.LockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

BOOL FilesVideoCapture::ReleaseFrame(CSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mBufferManager.UnlockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

HRESULT FilesVideoCapture::StartCaptureWithParam(CAPTURECONFIG& params)
{
	BOOL bRet = FALSE;
	W2S wchar2char;
	std::string filename = wchar2char.to_bytes(params.filePath.c_str());

	bRet = initVideoContext(filename.c_str());
	if (bRet){
		params.width = mVideoDecodeCtx->width;
		params.height = mVideoDecodeCtx->height;
		params.pixelFormat = GetFourCCByPixFmt(mVideoDecodeCtx->pix_fmt);
		params.fps = mVideoDecodeCtx->framerate.num*1.0 / mVideoDecodeCtx->framerate.den;
	}

	return bRet?S_OK:E_FAIL;
}


HRESULT FilesVideoCapture::StopCapture()
{
	cleanUp();
	return S_OK;
}

