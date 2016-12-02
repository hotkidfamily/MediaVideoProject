#pragma once

#ifdef VIDEORENDER_EXPORTS
#define VIDEORENDER_API __declspec(dllexport)
#else
#define VIDEORENDER_API __declspec(dllimport)
#endif

#include "SampleBuffer.h"

class IRender{
protected:
	~IRender(){};

public:
	virtual BOOL Repaint() = 0;
	virtual BOOL InitRender(HWND, int32_t, int32_t, DWORD) = 0;
	virtual BOOL DeinitRender() = 0;

	virtual BOOL PushFrame(CSampleBuffer *frame) = 0;
	virtual const char *GetRenderDescriptor() = 0;
};

class IRenderFactory
{
protected:
	~IRenderFactory(){};

public:
	virtual BOOL CreateRenderObj(IRender *&) = 0;
	virtual BOOL DestoryRenderObj(IRender *) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	VIDEORENDER_API BOOL GetRenderFactoryObj(IRenderFactory *&);
	VIDEORENDER_API BOOL ReleaseRenderFactoryObj(IRenderFactory *);

#ifdef __cplusplus
}
#endif