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

BOOL IVPPFactoryImpl::CreateVPPObj(IVPP *&vpp)
{
	vpp = new FFmpegProcess();

	return vpp != NULL;
}


void IVPPFactoryImpl::DestoryVPPObj(IVPP *ctx)
{
	if (ctx){
		FFmpegProcess *vpp = static_cast<FFmpegProcess*>(ctx);
		delete vpp;
		vpp = nullptr;
	}
}