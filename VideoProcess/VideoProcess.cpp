// VideoProcess.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "IVPP.h"
#include "FFmpegProcess.h"

VPPFactory::VPPFactory()
{

}

VPPFactory::~VPPFactory()
{
	
}

IVPP* VPPFactory::CreateVPP()
{
	BOOL bRet = TRUE;
	IVPP *vpp = new FFmpegProcess();
	return vpp;
}


void VPPFactory::DestoryVPP(IVPP *ctx)
{
	if (ctx){
		FFmpegProcess *proc = static_cast<FFmpegProcess*>(ctx);
		delete proc;
		proc = NULL;
	}
}