#include "stdafx.h"
#include "filesVideoCapture.h"

DWORD WINAPI decodeThread(void *args)
{
	FilesVideoCapture *pCapture = (FilesVideoCapture *)args;
	return pCapture->DecodeLoop();
}

BOOL FilesVideoCapture::initVideoCodec()
{
	AVCodec *videoDec = NULL;
	BOOL bRet = FALSE;
	int32_t ret = -1;
	E_RES res;
	AVRational time_base;
	double rate = 0;

	ret = av_find_best_stream(mFileCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (ret < 0){
		logger(Error, "could not find audio stream.\n");
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
	mVideoFrameRate.num = time_base.num;
	mVideoFrameRate.den = time_base.den;
	rate = av_q2d(time_base);
	mVideStreamPtsStep = (int64_t)(av_q2d(mVideoStream->time_base) * 10000000);

	res = GetResByResolution(mVideoDecodeCtx->width, mVideoDecodeCtx->height);
	if (rate <= 120){
		mVideoSampleBufferManager.Reset(res, (int32_t)rate / 2);
	} else{
		logger(Info, "frame rate(%dfps) is too high, guess fps %f\n", rate,
			av_q2d(av_stream_get_r_frame_rate(mVideoStream)));
		mVideoSampleBufferManager.Reset(res, 10);
	}

	mVideoDecDestFrame = av_frame_alloc();
	if (!mVideoDecDestFrame) {
		logger(Error, "Could not allocate AVFrame.\n");
		goto fail;
	}

	bRet = TRUE;

fail:
	return bRet;
}


BOOL FilesVideoCapture::initAudioCodec()
{
	AVCodec *audioDec = NULL;
	BOOL bRet = FALSE;
	int32_t ret = -1;
	AVRational time_base;
	double rate = 0;

	ret = av_find_best_stream(mFileCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	if (ret < 0){
		logger(Error, "could not find audio stream.\n");
		goto fail;
	}

	mAudioStream = mFileCtx->streams[ret];
	mAudioStreamIndex = ret;

	mAudioDecodeCtx = mAudioStream->codec;
	audioDec = avcodec_find_decoder(mAudioStream->codec->codec_id);

	/* Init the decoders, with or without reference counting */
	if ((ret = avcodec_open2(mAudioDecodeCtx, audioDec, NULL)) < 0) {
		logger(Error, "Failed to open %s codec\n");
		goto fail;
	}

	time_base = av_guess_frame_rate(mFileCtx, mAudioStream, NULL);

	mAudioSampleBufferManager.Reset(1, 50);
	mAudioDecDestFrame = av_frame_alloc();
	if (!mAudioDecDestFrame) {
		logger(Error, "Could not allocate audio AVFrame.\n");
		goto fail;
	}

	bRet = TRUE;

fail:
	return bRet;
}

BOOL FilesVideoCapture::initFileParse(const char *filename)
{
	BOOL bRet = FALSE;

	mFrameIndex = 0;

	if (avformat_open_input(&mFileCtx, filename, NULL, NULL) < 0) {
		logger(Error, "Could not open source file %s\n", filename);
		goto fail;
	}

	if (avformat_find_stream_info(mFileCtx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		goto fail;
	}

	if (!initVideoCodec()){
		goto fail;
	}

	if (!initAudioCodec()){
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

	if (mAudioDecodeCtx){
		avcodec_close(mAudioDecodeCtx);
		mAudioDecodeCtx = NULL;
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
	
	if (mVideoDecDestFrame){
		av_frame_free(&mVideoDecDestFrame);
		mVideoDecDestFrame = NULL;
	}

	if (mAudioDecDestFrame){
		av_frame_free(&mAudioDecDestFrame);
		mAudioDecDestFrame = NULL;
	}

	mVideoStream = NULL;
	mAudioStream = NULL;
}

int32_t FilesVideoCapture::decodeVideoPacket(int *got_frame, AVPacket &pkt)
{
	int ret = 0;
	int decoded = pkt.size;
	QUEUE_RET q_ret = Q_SUCCESS;
	int64_t framePts = 0;

	*got_frame = 0;

	/* decode video frame */
	ret = avcodec_decode_video2(mVideoDecodeCtx, mVideoDecDestFrame, got_frame, &pkt);
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
		desc.width = mVideoDecDestFrame->width;
		desc.height = mVideoDecDestFrame->height;
		desc.pixelFormatInFourCC = GetFourCCByPixFmt(mVideoDecDestFrame->format);
		framePts = av_frame_get_best_effort_timestamp(mVideoDecDestFrame) * mVideStreamPtsStep;
		desc.ptsStart = mBaseClock->GetBaseTime() + framePts;
		desc.ptsEnd = desc.ptsStart + av_frame_get_pkt_duration(mVideoDecDestFrame);
		logger(Debug, "pts %lld, diff %lld \n", desc.ptsStart, desc.ptsStart - mLastVideoFramePts);
		desc.frameStartIdx = mFrameIndex++;
		desc.frameEndIdx = mFrameIndex;
		mLastVideoFramePts = desc.ptsStart;

		desc.colorRange = (ColorRange)av_frame_get_color_range(mVideoDecDestFrame);
		desc.colorPrimaries = (ColorPrimaries)mVideoDecDestFrame->color_primaries;
		desc.colorTransfer = (ColorTransfer)mVideoDecDestFrame->color_trc;

		int buf_len = av_image_get_buffer_size((AVPixelFormat)mVideoDecDestFrame->format, mVideoDecDestFrame->width, mVideoDecDestFrame->height, 1);
		if (mDecDestCopiedBufferSize < buf_len){
			if(ReallocMemory(&mDecDestCopiedBuffer, buf_len))
				mDecDestCopiedBufferSize = buf_len;
		}

		ret = av_image_copy_to_buffer(mDecDestCopiedBuffer, mDecDestCopiedBufferSize, mVideoDecDestFrame->data,
			mVideoDecDestFrame->linesize, (AVPixelFormat)mVideoDecDestFrame->format, mVideoDecDestFrame->width, mVideoDecDestFrame->height, 1);

		if (ret > 0){
			desc.dataPtr = mDecDestCopiedBuffer;
			desc.validDataSize = ret;
		}
		// push frame to queue
		while ((q_ret = mVideoSampleBufferManager.FillFrame(desc)) != Q_SUCCESS){
			if (mDecodeThreadQuit){
				logger(Error, "End decode for exit thread.\n");
				break;
			}

			if (q_ret == Q_FULL){
				logger(Debug, "Video Decode Queue full.\n");
				Sleep(1);
				continue;
			} else {
				logger(Error, "Can not push video frame as: Queue is %d\n", q_ret);
				break;
			}
		}
	}

fail:
	return decoded;
}

int32_t FilesVideoCapture::decodeAudioPacket(int *got_frame, AVPacket &pkt)
{
	int ret = 0;
	int decoded = 0;
	QUEUE_RET q_ret = Q_SUCCESS;
	int64_t framePts = 0;

	*got_frame = 0;

	/* decode video frame */
	ret = avcodec_decode_audio4(mAudioDecodeCtx, mAudioDecDestFrame, got_frame, &pkt);
	if (ret < 0) {
		char erro[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		av_make_error_string(erro, AV_ERROR_MAX_STRING_SIZE, ret);
		logger(Error, "decode audio error %s.\n", erro);
		goto fail;
	}

	decoded = FFMIN(ret, pkt.size);

	if (*got_frame) {
		AudioSampleBuffer desc;
		int32_t unpadded_linesize = mAudioDecDestFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)mAudioDecDestFrame->format);
		desc.dataPtr = mAudioDecDestFrame->extended_data[0];
		desc.validDataSize = unpadded_linesize;
		desc.channels = mAudioDecodeCtx->channels;
		desc.sampleRate = mAudioDecodeCtx->sample_rate;
		desc.sampleFormat = mAudioDecodeCtx->sample_fmt;
		desc.bitsPerSample = mAudioDecodeCtx->bits_per_raw_sample;

		// push frame to queue
		while ((q_ret = mAudioSampleBufferManager.FillFrame(desc)) != Q_SUCCESS){
			if (mDecodeThreadQuit){
				logger(Error, "End decode for exit thread.\n");
				break;
			}

			if (q_ret == Q_FULL){
				logger(Debug, "Audio Decode Queue full.\n");
				Sleep(1);
				continue;
			} else {
				logger(Error, "Can not push audio frame as: Queue is %d\n", q_ret);
				break;
			}
		}
	}

	ret = decoded;

fail:
	return ret;
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
			if (pkt.stream_index == mVideoStreamIndex){
				AVPacket orig_pkt = pkt;
				do {
					if (mDecodeThreadQuit){
						logger(Error, "thread end by exit.\n");
						break;
					}
					ret = decodeVideoPacket(&got_frame, pkt);
					if (ret < 0)
						break;
					pkt.data += ret;
					pkt.size -= ret;
				} while (pkt.size > 0);
				av_packet_unref(&orig_pkt);
			} else if (pkt.stream_index == mAudioStreamIndex){
				AVPacket orig_pkt = pkt;
				do {
					if (mDecodeThreadQuit){
						logger(Error, "thread end by exit.\n");
						break;
					}
					ret = decodeAudioPacket(&got_frame, pkt);
					if (ret < 0)
						break;
					pkt.data += ret;
					pkt.size -= ret;
				} while (pkt.size > 0);
				av_packet_unref(&orig_pkt);
			} else{
				av_packet_unref(&pkt);
				continue;
			}
		} else{
			/* flush cached frames */
			pkt.data = NULL;
			pkt.size = 0;
			do {
				decodeVideoPacket(&got_frame, pkt);
			} while (got_frame);

			pkt.data = NULL;
			pkt.size = 0;
			do {
				decodeAudioPacket(&got_frame, pkt);
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
	, mVideoDecDestFrame(NULL)
	, mFrameIndex(0)
	, mVideoDecodeCtx(NULL)
	, mVideoStreamIndex(0)
	, mBaseClock(&clock)
	, mVideoStream(NULL)
	, mLastVideoFramePts(0)
	, mbDecodeLoop(FALSE)
	, mAudioDecDestFrame(NULL)
	, mAudioDecodeCtx(NULL)
	, mAudioStreamIndex(0)
	, mAudioStream(NULL)
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
	if (mVideoSampleBufferManager.LockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

BOOL FilesVideoCapture::ReleaseFrame(VideoSampleBuffer *&pSample)
{
	BOOL bRet = FALSE;
	if (mVideoSampleBufferManager.UnlockFrame(pSample)){
		bRet = TRUE;
	}

	return bRet;
}

HRESULT FilesVideoCapture::StartCaptureWithParam(CAPTURECONFIG& params)
{
	BOOL bRet = FALSE;
	W2S wchar2char;
	std::string filename = wchar2char.to_bytes(params.filePath.c_str());

	bRet = initFileParse(filename.c_str());
	if (bRet){
		params.width = mVideoDecodeCtx->width;
		params.height = mVideoDecodeCtx->height;
		params.pixelFormat = GetFourCCByPixFmt(mVideoDecodeCtx->pix_fmt);
		params.fps = mVideoFrameRate;
	}

	return bRet?S_OK:E_FAIL;
}


HRESULT FilesVideoCapture::StopCapture()
{
	cleanUp();
	return S_OK;
}

