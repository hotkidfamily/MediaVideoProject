#include "stdafx.h"
#include "encoder_x264.h"

#pragma  comment(lib, "comsuppw.lib")
#pragma comment(lib, "libx264.lib")

CLibx264::CLibx264()
	: mCodecHandle(nullptr)
	, mVppFactory(nullptr)
	, mVpp(nullptr)
{

}

CLibx264::~CLibx264()
{
	close();
}

bool CLibx264::open()
{
	BOOL bRet = FALSE;

	if (mbNeedVpp){
		if (!mVppFactory)
			bRet = GetVPPFactoryObj(mVppFactory);

		if (mVppFactory){
			bRet = mVppFactory->CreateVPPObj(mVpp);
		}

		if (!mVppFactory || !mVpp){
			return false;
		}

		x264_picture_alloc(&mInPic, mCodecParams.i_csp, mCodecParams.i_width, mCodecParams.i_height);
	}

	mCodecHandle = x264_encoder_open(&mCodecParams);
	if (!mCodecHandle)
	{
		x264_encoder_close(mCodecHandle);
		setLastError(-1);
	}

	mPackages.Reset(10);

	return (mCodecHandle != nullptr);
}

void CLibx264::close()
{
	if (mCodecHandle != nullptr){
		flushEncodeCache();
		x264_encoder_close(mCodecHandle);
		mCodecHandle = nullptr;
	}

	if (mVpp) {
		mVpp->DeinitContext();
	}

	if (mVppFactory){
		mVppFactory->DestoryVPPObj(mVpp);
		ReleaseVPPFactoryObj(mVppFactory);
		mVpp = nullptr;
		mVppFactory = nullptr;
	}

	x264_picture_clean(&mInPic);
}

bool CLibx264::setConfig(const ENCODECCFG &config)
{
	mWorkConfig = config;

	ZeroMemory(&mCodecParams, sizeof(x264_param_t));

	x264_param_default(&mCodecParams);

	mCodecParams.rc.i_vbv_max_bitrate = config.maxBitrateInKb;
	mCodecParams.rc.i_vbv_buffer_size = config.maxBitrateInKb;
	mCodecParams.rc.i_bitrate = config.avgBitrateInKb;
	mCodecParams.rc.i_rc_method = X264_RC_ABR;

	switch (config.pixelFormat){
	case PIXEL_FORMAT_RGB24:
		mCodecParams.i_csp = X264_CSP_BGR;
		break;
	case PIXEL_FORMAT_I420:
		mCodecParams.i_csp = X264_CSP_I420;
		break;
	case PIXEL_FORMAT_RGB32:
		mCodecParams.i_csp = X264_CSP_BGRA;
		break;
	case PIXEL_FORMAT_YUY2:
		mbNeedVpp = TRUE;
		mCodecParams.i_csp = X264_CSP_NV16;
		break;
	}

	if (mbNeedVpp){
		vppParams.inDesc.width = config.width;
		vppParams.inDesc.height = config.height;
		vppParams.inDesc.pixelFormat = config.pixelFormat;
		vppParams.outDesc.width = config.width;
		vppParams.outDesc.height = config.height;
		vppParams.outDesc.pixelFormat = config.pixelFormat;
		vppParams.flags = 0x10/*SWS_POINT*/;
	}

	mCodecParams.b_annexb = 1;
	mCodecParams.i_width = config.width;
	mCodecParams.i_height = config.height;

	mCodecParams.i_fps_num = config.fps.num;
	mCodecParams.i_fps_den = config.fps.den;

//  BUGS:
// 	mCodecParams.i_timebase_den = 1 * 1000 * 1000 * 10; // 10MHz
// 	mCodecParams.i_timebase_num = (uint32_t)(mCodecParams.i_timebase_den / (mCodecParams.i_fps_num*1.0/mCodecParams.i_fps_den));
	
 	mCodecParams.i_timebase_den = 1000;
 	mCodecParams.i_timebase_num = (uint32_t)(mCodecParams.i_timebase_den / (mCodecParams.i_fps_num*1.0/mCodecParams.i_fps_den));

	mCodecParams.b_vfr_input = 1;
	mCodecParams.b_repeat_headers = 1;	

	parseConfigString();
	return true;
}

bool CLibx264::reset(const ENCODECCFG &config)
{
	return false;
}

bool CLibx264::addFrame(const VideoSampleBuffer &inputFrame)
{
	x264_picture_t inpic;
	x264_picture_init(&inpic);

	switch (inputFrame.pixelFormatInFourCC)
	{
	case PIXEL_FORMAT_RGB24:
		inpic.img.i_csp = X264_CSP_BGR;
		inpic.img.i_plane = 1;
		inpic.img.plane[0] = inputFrame.planarPtr[0];
		inpic.img.i_stride[0] = inputFrame.planarStride[0];
		break;
	case PIXEL_FORMAT_I420:
		inpic.img.i_csp = X264_CSP_I420;
		inpic.img.i_plane = 3;
		inpic.img.plane[0] = inputFrame.planarPtr[0];
		inpic.img.plane[1] = inputFrame.planarPtr[1];
		inpic.img.plane[2] = inputFrame.planarPtr[2];
		inpic.img.i_stride[0] = inputFrame.planarStride[0];
		inpic.img.i_stride[1] = inputFrame.planarStride[1];
		inpic.img.i_stride[2] = inputFrame.planarStride[2];
		break;
	case PIXEL_FORMAT_RGB32: 
		inpic.img.i_csp = X264_CSP_BGRA;
		inpic.img.i_plane = 1;
		inpic.img.plane[0] = inputFrame.planarPtr[0];
		inpic.img.i_stride[0] = inputFrame.planarStride[0];
		break;
	case PIXEL_FORMAT_YUY2:
		//mVpp->convertYUY2toNV16(mInPic, &inputFrame);
		break;
	default:
		return false;
	}

	if (inputFrame.pixelFormatInFourCC == PIXEL_FORMAT_YUY2){
		int64_t ptss = 0, ptse = 0;
		mInPic.i_pts = inputFrame.ptsStart / 10000;
		
		mInPic.i_type = X264_TYPE_AUTO;
		encodeFrame(&mInPic);
	} else {
		int64_t ptss = 0, ptse = 0;
		inpic.i_pts = inputFrame.ptsStart / 10000;
		inpic.i_type = X264_TYPE_AUTO;
		encodeFrame(&inpic);
	}
	
	return true;
}

bool CLibx264::getPackage(CPackageBuffer *&outputPackage)
{
	return !!mPackages.LockPackage(outputPackage);
}

bool CLibx264::releasePackage(CPackageBuffer *&outputPackage)
{
	return !!mPackages.UnlockPackage(outputPackage);
}

bool CLibx264::encodeFrame(x264_picture_t *inpic)
{
	bool ret = false;
	int outputNALsDataSizeInBytes = 0;
	x264_nal_t *outputNalus = nullptr;
	int outputNaluCnt = 0;
	x264_picture_t outputPic;
	x264_picture_init(&outputPic);

	outputNALsDataSizeInBytes = x264_encoder_encode(
		mCodecHandle, &outputNalus, &outputNaluCnt, inpic, &outputPic);

	ret = assemblePackage(outputNALsDataSizeInBytes, outputNalus, outputNaluCnt, &outputPic);

	return ret;
}

bool CLibx264::assemblePackage(int uNALsDataSizeInBytes, 
	const x264_nal_t *outputNalus,
	int outputNaluCnt,
	const x264_picture_t *outputPic)
{
	bool ret = true;
	FRAME_TYPE frameType;
	uint8_t *pOutData = nullptr;
	uint8_t *pExtraData = nullptr;
	uint32_t extraDataSize = 0;
	CPackageBuffer *outPackage = nullptr;

	if (uNALsDataSizeInBytes > 0)// encode one frame.
	{
		int i = 0;
		if (!mPackages.GetPackage(outPackage)){
			return false;
		}

		// 1.calculate buffer size 
		for (i = 0; i < outputNaluCnt; i++){
			switch (outputNalus[i].i_type){
			case NAL_SPS:
			case NAL_PPS:
				extraDataSize += outputNalus[i].i_payload;
			default:
				break;
			}
		}
		if (!outPackage->Reset(uNALsDataSizeInBytes, uNALsDataSizeInBytes - extraDataSize)){
			return false;
		}

		pOutData = outPackage->Data();
		pExtraData = outPackage->ExtraData();

		// 3.copy data
		for (i = 0; i < outputNaluCnt; i++){
			switch (outputNalus[i].i_type){
			case NAL_SPS:
			case NAL_PPS:
				memcpy(pExtraData, outputNalus[i].p_payload, outputNalus[i].i_payload);
				pExtraData += outputNalus[i].i_payload;
				break;
			default:
				memcpy(pOutData, outputNalus[i].p_payload, outputNalus[i].i_payload);
				pOutData += outputNalus[i].i_payload;
				break;
			}
		}

		switch (outputPic->i_type){
		case X264_TYPE_IDR:
		case X264_TYPE_I:
		case X264_TYPE_P:
			frameType = (FRAME_TYPE)outputPic->i_type;
			break;
		case X264_TYPE_BREF: // make b_ref as P ?
			frameType = P_FRAME;
			break;
		case X264_TYPE_B:
			frameType = B_FRAME;
			break;
		default: /* just header ? Fix ME:*/
			frameType = ERR_FRAME;
			break;
		}

		outPackage->SetPts(outputPic->i_pts);
		outPackage->SetDts(outputPic->i_dts);
		outPackage->SetFrameType(frameType);

	}else if (uNALsDataSizeInBytes == 0){
		// cache
	}else{
		ret = false;
	}

	return ret;
}

void CLibx264::flushEncodeCache()
{
	int ret = 0;
	int eStatus = 0;
	x264_nal_t * outputNalus = nullptr;
	int outputNaluCount = 0;
	x264_picture_t outputPic;

	x264_picture_init(&outputPic);

	if (1){
		// flush delay frames
		while (x264_encoder_delayed_frames(mCodecHandle)){
			eStatus = x264_encoder_encode(mCodecHandle, &outputNalus, &outputNaluCount, nullptr, &outputPic);

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
