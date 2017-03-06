#include "stdafx.h"
#include "AudioRender.h"
#include "syncrender.h"
#include "DdrawRender.h"
#include "D3D9Render.h"

IAduioRenderFactoryImpl::IAduioRenderFactoryImpl()
{

}

IAduioRenderFactoryImpl::~IAduioRenderFactoryImpl()
{

}

BOOL IAduioRenderFactoryImpl::CreateRenderObj(IAudioRender *&render)
{
	render = new D3D9Render();

	return render != nullptr;
}

BOOL IAduioRenderFactoryImpl::DestoryRenderObj(IAudioRender *render)
{
	if (render)
		delete static_cast<D3D9Render*>(render);

	return TRUE;
}