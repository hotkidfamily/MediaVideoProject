// VideoProcess.cpp : 定义 DLL 应用程序的导出函数。
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

IVPP* IVPPFactoryImpl::CreateVPPObj()
{
	BOOL bRet = TRUE;
	IVPP *vpp = new FFmpegProcess();
	return vpp;
}


void IVPPFactoryImpl::DestoryVPPObj(IVPP *ctx)
{
	if (ctx){
		FFmpegProcess *vpp = static_cast<FFmpegProcess*>(ctx);
		delete vpp;
		vpp = NULL;
	}
}