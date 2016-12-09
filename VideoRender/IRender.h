#pragma once

#ifdef VIDEORENDER_EXPORTS
#define VIDEORENDER_API __declspec(dllexport)
#else
#define VIDEORENDER_API __declspec(dllimport)
#endif

#include "SampleBuffer.h"

struct RENDERCONFIG
{
	RENDERCONFIG()
	{
		ZeroMemory(this, sizeof(struct RENDERCONFIG));
	}

	HWND hWnd; /* NULL£º full screen, else windowed */
	int32_t width;
	int32_t height;
	DWORD pixelFormat;
	BOOL bWaitVSync;
	struct 
	{
		double num;
		int32_t den;
	}fps;
};

class IRender{
protected:
	~IRender(){};

public:
	virtual BOOL Repaint() = 0;
	virtual BOOL InitRender(const RENDERCONFIG &) = 0;
	virtual BOOL DeinitRender() = 0;

	virtual BOOL PushFrame(CSampleBuffer *) = 0;
	virtual const char *GetRenderDescriptor() const = 0;
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