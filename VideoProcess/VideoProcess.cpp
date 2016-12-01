// VideoProcess.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "VideoProcess.h"
#include "FFmpegProcess.h"

IVPPFactoryImpl::IVPPFactoryImpl()
{

}

IVPPFactoryImpl::~IVPPFactoryImpl()
{
	
}

IVPP* IVPPFactoryImpl::CreateVPP()
{
	BOOL bRet = TRUE;
	IVPP *vpp = new FFmpegProcess();
	return vpp;
}


void IVPPFactoryImpl::DestoryVPP(IVPP *ctx)
{
	if (ctx){
		FFmpegProcess *vpp = static_cast<FFmpegProcess*>(ctx);
		delete vpp;
		vpp = NULL;
	}
}