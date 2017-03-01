#include "stdafx.h"
#include "VideoRender.h"
#include "syncrender.h"
#include "DdrawRender.h"
#include "D3D9Render.h"

IRenderFactoryImpl::IRenderFactoryImpl()
{

}

IRenderFactoryImpl::~IRenderFactoryImpl()
{

}

BOOL IRenderFactoryImpl::CreateRenderObj(IRender *&render)
{
	render = new D3D9Render();

	return render != nullptr;
}

BOOL IRenderFactoryImpl::DestoryRenderObj(IRender *render)
{
	if (render)
		delete static_cast<D3D9Render*>(render);

	return TRUE;
}