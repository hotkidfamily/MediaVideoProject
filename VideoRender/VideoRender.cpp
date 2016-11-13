// VideoRender.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "VideoRender.h"

HRESULT IRenderDelegate::Repaint()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT IRenderDelegate::InitRender()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IRenderDelegate::DeinitRender()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IRenderDelegate::PushFrame(RENDER_FRAME &frame)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT IRenderDelegate::GetRenderNameStr(const char* &name)
{
	HRESULT hr = S_OK;
	return E_FAIL;
}

