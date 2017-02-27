#include "stdafx.h"
#include "filesVideoCapture.h"
#include "logger.h"


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

	videoDec = avcodec_find_decoder(vs->codec->codec_id);
	mVideoDecodeCtx = avcodec_alloc_context3(videoDec);
	if (!mVideoDecodeCtx){
		logger(Error, "could not open decoder for file.\n");
		goto fail;
	}

	avcodec_copy_context(mVideoDecodeCtx, vs->codec);

	if ((ret = avcodec_open2(mVideoDecodeCtx, videoDec, NULL)) < 0) {
		logger(Error, "Failed to open %s codec\n");
		goto fail;
	}

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
	if (mVideoDecodeCtx){
		avcodec_free_context(&mVideoDecodeCtx);
	}
	mVideoDecodeCtx = NULL;

	if (mFileCtx){
		avformat_close_input(&mFileCtx);
	}
	mFileCtx = NULL;
	
	if (mDecDestCopiedBuffer)
		DeallocMemory(mDecDestCopiedBuffer);

	mDecDestCopiedBuffer = NULL;
	mDecDestCopiedBufferSize = 0;
}

int32_t FilesVideoCapture::decodePacket(int *got_frame, int cached, AVPacket &pkt)
{
	int ret = 0;
	int decoded = pkt.size;

	*got_frame = 0;

	/* decode video frame */
	ret = avcodec_decode_video2(mVideoDecodeCtx, mDecDestFrame, got_frame, &pkt);
	if (ret < 0) {
		logger(Error, "decode video error.");
		goto fail;
	}

	if (*got_frame) {
		FRAME_DESC desc;

		desc.width = mVideoDecodeCtx->width;
		desc.height = mVideoDecodeCtx->height;
		desc.pixelFormatInFourCC = mVideoDecodeCtx->pix_fmt;
		desc.ptsStart = av_frame_get_best_effort_timestamp(mDecDestFrame);
		desc.ptsEnd = av_frame_get_best_effort_timestamp(mDecDestFrame);
		desc.frameStartIdx = mFrameIndex++;
		desc.frameEndIdx = mFrameIndex;

		desc.colorRange = (ColorRange)mDecDestFrame->color_range;
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

	while (av_read_frame(mFileCtx, &pkt)){
		AVPacket orig_pkt = pkt;
		do {
			ret = decodePacket(&got_frame, 0, pkt);
			if (ret < 0)
				break;
			pkt.data += ret;
			pkt.size -= ret;
		} while (pkt.size > 0);
		av_packet_unref(&orig_pkt);
	}

	/* flush cached frames */
	pkt.data = NULL;
	pkt.size = 0;
	do {
		decodePacket(&got_frame, 1, pkt);
	} while (got_frame);
	
}


FilesVideoCapture::FilesVideoCapture()
{
	av_register_all();
}

FilesVideoCapture::~FilesVideoCapture()
{
	cleanUp();
}

