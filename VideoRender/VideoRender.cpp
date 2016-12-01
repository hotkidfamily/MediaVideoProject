#include "stdafx.h"
#include "VideoRender.h"
#include "DdrawRender.h"

IRenderFactoryImpl::IRenderFactoryImpl()
{

}

IRenderFactoryImpl::~IRenderFactoryImpl()
{

}

BOOL IRenderFactoryImpl::CreateRenderObj(IRender *render)
{
	render = new DDrawRender();

	return render != NULL;
}

BOOL IRenderFactoryImpl::DestoryRenderObj(IRender *render)
{
	if (render)
		delete static_cast<DDrawRender*>(render);

	return TRUE;
}