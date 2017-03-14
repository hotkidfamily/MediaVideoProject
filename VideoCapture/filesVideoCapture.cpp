#include "stdafx.h"
#include "filesVideoCapture.h"

DWORD WINAPI decodeThread(void *args)
{
	FilesVideoCapture *pCapture = (FilesVideoCapture *)args;
	return pCapture->DecodeLoop();
}

BOOL FilesVideoCapture::initVideoContext(const char *filename)
{
	AVCodec *videoDec = NULL;
	BOOL bRet = FALSE;
	int ret = -1;
	AVRational time_base;
	E_RES res;
	double rate = 0;

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

	mVideoStream = mFileCtx->streams[ret];
	mVideoStreamIndex = ret;

	mVideoDecodeCtx = mVideoStream->codec;
	videoDec = avcodec_find_decoder(mVideoStream->codec->codec_id);

	/* Init the decoders, with or without reference counting */
	if ((ret = avcodec_open2(mVideoDecodeCtx, videoDec, NULL)) < 0) {
		logger(Error, "Failed to open %s codec\n");
		goto fail;
	}

	time_base = av_guess_frame_rate(mFileCtx, mVideoStream, NULL);
	mFrameRate.num = time_base.num;
	mFrameRate.den = time_base.den;
	rate = av_q2d(time_base);
	mVideStreamPtsStep = (int64_t)(av_q2d(mVideoStream->time_base) * 10000000);

	res = GetResByResolution(mVideoDecodeCtx->width, mVideoDecodeCtx->height);
	if (rate <= 120){
		mBufferManager.Reset(res, (rate/2));
	} else{
		logger(Info, "frame rate(%dfps) is too high, guess fps %f\n", rate, 
			av_q2d(av_stream_get_r_frame_rate(mVideoStream)));
		mBufferManager.Reset(res, 10);
	}

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

	mVideoStream = NULL;
}

int32_t FilesVideoCapture::decodePacket(int *got_frame, AVPacket &pkt)
{
	int ret = 0;
	int decoded = pkt.size;
	QUEUE_RET q_ret = Q_SUCCESS;
	int64_t framePts = 0;

	*got_frame = 0;

	/* decode video frame */
	ret = avcodec_decode_video2(mVideoDecodeCtx, mDecDestFrame, got_frame, &pkt);
	if (ret < 0) {
		logger(Error, "decode video error.\n");
		goto fail;
	}

	if (*got_frame) {
		VideoSampleBuffer desc;
		if (mbDecodeLoop){
			mBaseClock->ResetBaseTime(mLastVideoFramePts);
			mbDecodeLoop = FALSE;
		}
		desc.width = mDecDestFrame->width;
		desc.height = mDecDestFrame->height;
		desc.pixelFormatInFourCC = GetFourCCByPixFmt(mDecDestFrame->format);
		framePts = av_frame_get_best_effort_timestamp(mDecDestFrame) * mVideStreamPtsStep;
		desc.ptsStart = mBaseClock->GetBaseTime() + framePts;
		desc.ptsEnd = desc.ptsStart + av_frame_get_pkt_duration(mDecDestFrame);
		logger(Debug, "pts %lld, diff %lld \n", desc.ptsStart, desc.ptsStart - mLastVideoFramePts);
		desc.frameStartIdx = mFrameIndex++;
		desc.frameEndIdx = mFrameIndex;
		mLastVideoFramePts = desc.ptsStart;

		desc.colorRange = (ColorRange)av_frame_get_color_range(mDecDestFrame);
		desc.colorPrimaries = (ColorPrimaries)mDecDestFrame->color_primaries;
		desc.colorTransfer = (ColorTransfer)mDecDestFrame->color_trc;

		int buf_len = av_image_get_buffer_size((AVPixelFormat)mDecDestFrame->format, mDecDestFrame->width, mDecDestFrame->height, 1);
		if (mDecDestCopiedBufferSize < buf_len){
			if(ReallocMemory(&mDecDestCopiedBuffer, buf_len))
				mDecDestCopiedBufferSize = buf_len;
		}

		ret = av_image_copy_to_buffer(mDecDestCopiedBuffer, mDecDestCopiedBufferSize, mDecDestFrame->data,
			mDecDestFrame->linesize, (AVPixelFormat)mDecDestFrame->format, mDecDestFrame->width, mDecDestFrame->height, 1);

		if (ret > 0){
			desc.dataPtr = mDecDestCopiedBuffer;
			desc.validDataSize = ret;
		}
		// push frame to queue
		while ((q_ret = mBufferManager.FillFrame(desc)) != Q_SUCCESS){
			if (mDecodeThreadQuit){
				logger(Error, "End decode for exit thread.\n");
				break;
			}

			if (q_ret == Q_FULL){
				logger(Debug, "Decode Queue full.\n");
				Sleep(1);
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
				av_packet_unref(&pkt);
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
		} else{
			/* flush cached frames */
			pkt.data = NULL;
			pkt.size = 0;
			do {
				decodePacket(&got_frame, pkt);
			} while (got_frame);

			av_seek_frame(mFileCtx, mVideoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
			mbDecodeLoop = TRUE;
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
	, mVideoStream(NULL)
	, mLastVideoFramePts(0)
	, mbDecodeLoop(FALSE)
{
	av_register_all();
}

FilesVideoCapture::~FilesVideoCapture()
{
	cleanUp();
}

BOOL FilesVideoCapture::GetFrame(VideoSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mBufferManager.LockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

BOOL FilesVideoCapture::ReleaseFrame(VideoSampleBuffer *&pSample)
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
		params.fps = mFrameRate;
	}

	return bRet?S_OK:E_FAIL;
}


HRESULT FilesVideoCapture::StopCapture()
{
	cleanUp();
	return S_OK;
}

