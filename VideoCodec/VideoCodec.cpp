// VideoCodec.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "VideoCodec.h"
#include "encoder_x264.h"

enum CodecID{
	VCODEC_NONE,
	VCODEC_H264,
	VCODEC_AVC = VCODEC_H264,
	VCODEC_H265,
	VCODEC_HEVC = VCODEC_H265,
	VCODEC_VP8,
	VCODEC_VP9,
};

const struct tagCodecList
{
	DWORD codecId;
	DWORD priority;
} CodecList[] = {
	{ VCODEC_H264, 1 },
	{ VCODEC_HEVC, 2 },
	{ VCODEC_VP8, -2 },
	{ VCODEC_VP9, -1 },
};

ICodecFactoryImpl::ICodecFactoryImpl()
{

}

ICodecFactoryImpl::~ICodecFactoryImpl()
{

}

BOOL ICodecFactoryImpl::CreateCodec(ICodec* &codec)
{
	BOOL bRet = TRUE;
	codec = new CLibx264();
	return bRet;
}

BOOL ICodecFactoryImpl::DestoryCodec(ICodec *&codec)
{
	BOOL bRet = TRUE;

	if (codec)
		delete static_cast<CLibx264*>(codec);

	codec = NULL;
	return bRet;
}

BOOL ICodecFactoryImpl::UpdateCodecAbilityTable()
{
	return TRUE;
}
