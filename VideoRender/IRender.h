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
	virtual HRESULT Repaint() = 0;
	virtual HRESULT InitRender(int, int, DWORD) = 0;
	virtual HRESULT DeinitRender() = 0;

	virtual HRESULT PushFrame(CSampleBuffer *frame) = 0;
	virtual const char *GetRenderDescriptor() = 0;
};

class IRenderFactory
{
protected:
	~IRenderFactory(){};

public:
	virtual BOOL CreateRenderObj(IRender *) = 0;
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