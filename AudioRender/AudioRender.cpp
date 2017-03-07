#include "stdafx.h"
#include "AudioRender.h"
#include "syncrender.h"
#include "DdrawRender.h"
#include "D3DSoundRender.h"

IAduioRenderFactoryImpl::IAduioRenderFactoryImpl()
{

}

IAduioRenderFactoryImpl::~IAduioRenderFactoryImpl()
{

}

BOOL IAduioRenderFactoryImpl::CreateRenderObj(IAudioRender *&render)
{
	render = new D3DSoundRender();

	return render != nullptr;
}

BOOL IAduioRenderFactoryImpl::DestoryRenderObj(IAudioRender *render)
{
	if (render)
		delete static_cast<D3DSoundRender*>(render);

	return TRUE;
}