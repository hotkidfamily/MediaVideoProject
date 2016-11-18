#include "stdafx.h"
#include "encoder_x264.h"

BOOL convertYUY2toI420(x264_picture_t &dstPic, const CSampleBuffer *srcPic)
{
	// yuyv ==> y + u + v
	uint8_t *y = NULL, *u = NULL, *v = NULL;
	uint8_t *yp = NULL;
	int step = 0;

	int32_t width = srcPic->GetWidth();
	int32_t height = srcPic->GetHeight();
	int32_t linSize = srcPic->GetLineSize();

	y = dstPic.img.plane[0];
	yp = srcPic->GetDataPtr();
	for (int32_t i = 0; i < height*width; i++){
		y[i] = yp[i * 2];
	}

	u = dstPic.img.plane[1];
	v = dstPic.img.plane[2];
	yp = srcPic->GetDataPtr();
	for (int32_t j = 0; j < height; j += 2){
		yp = srcPic->GetDataPtr() + j*linSize;
		u = dstPic.img.plane[1] + j/2*dstPic.img.i_stride[1];
		v = dstPic.img.plane[2] + j/2*dstPic.img.i_stride[2];
		for (int32_t i = 0; i < width / 2; i++){
			u[i] = (yp[i * 4 + 1] + yp[i * 4 + linSize + 1])/2;
			v[i] = (yp[i * 4 + 3] + yp[i * 4 + linSize + 3])/2;
		}
	}

	return TRUE;
}

BOOL convertYUY2toNV16(x264_picture_t &dstPic, const CSampleBuffer *srcPic)
{
	uint8_t *y = NULL, *uv = NULL;
	uint8_t *yp = NULL;
	int step = 0;

	int32_t width = srcPic->GetWidth();
	int32_t height = srcPic->GetHeight();
	int32_t linSize = srcPic->GetLineSize();

	for (int32_t j = 0; j < height; j ++){
		y = dstPic.img.plane[0] + j * dstPic.img.i_stride[0];
		uv = dstPic.img.plane[1] + j * dstPic.img.i_stride[1];
		yp = srcPic->GetDataPtr() + j * linSize;

		for (int32_t i = 0, k= 0; i < linSize; i+=4, k+=2){
			y[k] = yp[i];
			uv[k] = yp[i + 1];
			y[k + 1] = yp[i + 2];
			uv[k + 1] = yp[i + 3];
		}
	}

	return TRUE;
}

CLibx264::CLibx264()
	: mCodecHandle(NULL)
{

}

CLibx264::~CLibx264()
{
	close();
}

bool CLibx264::open()
{
	mCodecHandle = x264_encoder_open(&mCodecParams);
	if (!mCodecHandle)
	{
		x264_encoder_close(mCodecHandle);
		setLastError(-1);
	}

	return (mCodecHandle != NULL);
}

void CLibx264::close()
{
	if (mCodecHandle != NULL){
		flushEncodeCache();
		x264_encoder_close(mCodecHandle);
		mCodecHandle = NULL;
	}

	if (mPackages.size())
		mPackages.clear();

	x264_picture_clean(&mInPic);
}

bool CLibx264::setConfig(const ENCODEC_CFG &config)
{
	mWorkConfig = config;

	ZeroMemory(&mCodecParams, sizeof(x264_param_t));

 	x264_param_default(&mCodecParams);

	mCodecParams.rc.i_vbv_max_bitrate = config.maxBitrateInKb;
	mCodecParams.rc.i_vbv_buffer_size = config.maxBitrateInKb;
	mCodecParams.rc.i_bitrate = config.avgBitrateInKb;
	mCodecParams.rc.i_rc_method = X264_RC_ABR;

	switch (config.pixelFormatInFourCC){
	case PIXEL_FORMAT_RGB24:
		mCodecParams.i_csp = X264_CSP_RGB;
		break;
	case PIXEL_FORMAT_I420:
		mCodecParams.i_csp = X264_CSP_I420;
		break;
	case PIXEL_FORMAT_RGB32:
		mCodecParams.i_csp = X264_CSP_BGRA;
		break;
	case PIXEL_FORMAT_YUY2:
		mCodecParams.i_csp = X264_CSP_NV16;
		break;
	}

	mCodecParams.b_annexb = 1;
	mCodecParams.i_width = config.width;
	mCodecParams.i_height = config.height;

	mCodecParams.i_fps_num = config.fps;
	mCodecParams.i_fps_den = 1;
// 	mCodecParams.i_timebase_num = 1;
// 	mCodecParams.i_timebase_den = 10000000;// only in apple camera
	mCodecParams.b_open_gop = 0;
	mCodecParams.b_cabac = 1;
	mCodecParams.i_keyint_min = config.fps;
	mCodecParams.i_keyint_max = config.fps * 5;
	mCodecParams.i_frame_reference = 6;
	mCodecParams.i_threads = 0;

	mCodecParams.b_repeat_headers = 1;
	x264_picture_alloc(&mInPic, mCodecParams.i_csp, mCodecParams.i_width, mCodecParams.i_height);

	parseConfigString();
	return true;
}

bool CLibx264::reset(const ENCODEC_CFG &config)
{
	return false;
}

bool CLibx264::addFrame(const CSampleBuffer &inputFrame)
{
	x264_picture_t inpic;
	x264_picture_init(&inpic);

	switch (inputFrame.GetPixelFormat())
	{
	case PIXEL_FORMAT_RGB24:
		inpic.img.i_csp = X264_CSP_RGB;
		inpic.img.i_plane = 1;
		inpic.img.plane[0] = inputFrame.GetDataPtr();
		inpic.img.i_stride[0] = inputFrame.GetWidth()*3;
		break;
	case PIXEL_FORMAT_I420:
		inpic.img.i_csp = X264_CSP_I420;
		inpic.img.i_plane = 3;
		inpic.img.plane[0] = inputFrame.GetDataPtr();
		inpic.img.plane[1] = inpic.img.plane[0] + inputFrame.GetWidth() *inputFrame.GetHeight();
		inpic.img.plane[2] = inpic.img.plane[1] + inputFrame.GetWidth() *inputFrame.GetHeight() / 4;
		inpic.img.i_stride[0] = inputFrame.GetWidth();
		inpic.img.i_stride[1] = inputFrame.GetWidth()/2;
		inpic.img.i_stride[2] = inputFrame.GetWidth()/2;
		break;
	case PIXEL_FORMAT_RGB32:
		inpic.img.i_csp = X264_CSP_BGRA;
		inpic.img.i_plane = 1;
		inpic.img.plane[0] = inputFrame.GetDataPtr();
		inpic.img.i_stride[0] = inputFrame.GetWidth()*4;
		break;
	case PIXEL_FORMAT_YUY2:
		convertYUY2toNV16(mInPic, &inputFrame);
		break;
	default:
		return false;
	}

	if (inputFrame.GetPixelFormat() == PIXEL_FORMAT_YUY2){
		mInPic.i_pts = inputFrame.GetPts() / 10000;
		mInPic.i_type = X264_TYPE_AUTO;
		encodeFrame(&mInPic);
	}
	else{
		inpic.i_pts = inputFrame.GetPts() / 10000;
		inpic.i_type = X264_TYPE_AUTO;
		encodeFrame(&inpic);
	}
	
	return true;
}

bool CLibx264::getPackage(DwVideoPackage &outputPackage)
{
	lock.Lock();
	if (mPackages.size()){
		outputPackage = mPackages.front();
		mPackages.pop_front();
	}
	lock.Unlock();
	return true;
}

bool CLibx264::encodeFrame(x264_picture_t *inpic)
{
	bool ret = false;
	int outputNALsDataSizeInBytes = 0;
	x264_nal_t *outputNalus = NULL;
	int outputNaluCnt = 0;
	x264_picture_t outputPic;
	x264_picture_init(&outputPic);

	outputNALsDataSizeInBytes = x264_encoder_encode(
		mCodecHandle, &outputNalus, &outputNaluCnt, inpic, &outputPic);

	ret = assemblePackage(outputNALsDataSizeInBytes, outputNalus, outputNaluCnt, &outputPic);

	return ret;
}

bool CLibx264::assemblePackage(int outputNALsDataSizeInBytes, 
	const x264_nal_t *outputNalus,
	int outputNaluCnt,
	const x264_picture_t *outputPic)
{
	bool ret = true;

	if (outputNALsDataSizeInBytes > 0)// encode one frame.
	{
		int i = 0;
		DwVideoPackage encodedPackage;
		uint8_t *pOutData = NULL;
		uint8_t *pExtraData = NULL;

		// 1.calculate buffer size 
		for (i = 0; i < outputNaluCnt; i++){
			switch (outputNalus[i].i_type){
			case NAL_SPS:
			case NAL_PPS:
				encodedPackage.extraDataSize += outputNalus[i].i_payload;
			default:
				break;
			}
		}

		encodedPackage.packageDataSize = outputNALsDataSizeInBytes - encodedPackage.extraDataSize;
		// 2.alloc buffer
		encodedPackage.packageData = new uint8_t[encodedPackage.packageDataSize + 16];
		if (!encodedPackage.packageData){
			return false;
		}

		if (encodedPackage.extraDataSize){
			encodedPackage.extraData = new uint8_t[encodedPackage.extraDataSize + 16];
			if (!encodedPackage.extraData){
				delete encodedPackage.packageData;
				encodedPackage.packageData = NULL;
				encodedPackage.packageDataSize = 0;
				return false;
			}
		}

		pOutData = encodedPackage.packageData;
		pExtraData = encodedPackage.extraData;

		// 3.copy data
		for (i = 0; i < outputNaluCnt; i++){
			switch (outputNalus[i].i_type){
			case NAL_SPS:
			case NAL_PPS:
				memcpy(pExtraData, outputNalus[i].p_payload, outputNalus[i].i_payload);
				pExtraData += outputNalus[i].i_payload;
			default:
				memcpy(pOutData, outputNalus[i].p_payload, outputNalus[i].i_payload);
				pOutData += outputNalus[i].i_payload;
				break;
			}
		}

		encodedPackage.pts = outputPic->i_pts;
		encodedPackage.dts = outputPic->i_dts;

		switch (outputPic->i_type){
		case X264_TYPE_IDR:
			encodedPackage.frameType = DwVideoPackage::IDR_FRAME;
			break;
		case X264_TYPE_I:
			encodedPackage.frameType = DwVideoPackage::I_FRAME;
			break;
		case X264_TYPE_P:
			encodedPackage.frameType = DwVideoPackage::P_FRAME;
			break;
		case X264_TYPE_BREF: // make b_ref as P
		case X264_TYPE_B:
			encodedPackage.frameType = DwVideoPackage::B_FRAME;
			break;
		default: /* just header ?*/
			encodedPackage.frameType = DwVideoPackage::ERR_FRAME;
			break;
		}
		lock.Lock();
		mPackages.push_back(encodedPackage);
		lock.Unlock();
	}else if (outputNALsDataSizeInBytes == 0){
	}else{
		ret = false;
	}

	return ret;
}

void CLibx264::flushEncodeCache()
{
	int ret = 0;
	int eStatus = 0;
	x264_nal_t * outputNalus = NULL;
	int outputNaluCount = 0;
	x264_picture_t outputPic;

	x264_picture_init(&outputPic);

	if (1){
		// flush delay frames
		while (x264_encoder_delayed_frames(mCodecHandle)){
			eStatus = x264_encoder_encode(mCodecHandle, &outputNalus, &outputNaluCount, NULL, &outputPic);

			ret = assemblePackage(eStatus, outputNalus, outputNaluCount, &outputPic);
			if (ret < 0){
				break;
			}
		}
	}
}

#undef OPT_STR
#define OPT_STR(opt, param)                                                   \
	do {                                                                      \
		int parse_ret = X264_PARAM_BAD_NAME;                                    \
		if (param && (parse_ret = x264_param_parse(&mCodecParams, opt, param)) < 0) { \
		switch (parse_ret) {\
		case X264_PARAM_BAD_NAME:\
			fprintf(stderr, "Unknown option: %s.\n", opt);\
			break;\
		case X264_PARAM_BAD_VALUE:\
			fprintf(stderr, "Invalid value for %s: %s.\n", opt, param);\
			break;\
		default:\
			break;\
			}\
						}\
			} while (0)

#include <comutil.h>
bool CLibx264::parseConfigString()
{
	_bstr_t b(mWorkConfig.cfgStr.c_str());
	const char* pCsConfig = b;

	char param[256] = { '\0' }, val[256] = { '\0' };

	if (!pCsConfig || strlen(pCsConfig) <= 0){
		return false;
	}

	while (pCsConfig){
		if (sscanf_s(pCsConfig, "%255[^:=]=%255[^:=]", param, _countof(param), val, _countof(val)) == 1){
			OPT_STR(param, "1");
		}else{
			OPT_STR(param, val);
		}
		pCsConfig = strchr(pCsConfig, ':');
		pCsConfig += !!pCsConfig;
	}

	return true;
}
#undef OPT_STR
