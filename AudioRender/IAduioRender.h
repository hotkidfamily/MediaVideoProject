#pragma once

#ifdef AUDIORENDER_EXPORTS
#define AUDIORENDER_API __declspec(dllexport)
#else
#define AUDIORENDER_API __declspec(dllimport)
#endif

struct RENDERCONFIG
{
	RENDERCONFIG(){
		ZeroMemory(this, sizeof(struct RENDERCONFIG));
	}

	HWND hWnd; /* NULL£º full screen, else windowed */
	int32_t width;
	int32_t height;
	DWORD pixelFormat;
	struct 
	{
		double num;
		int32_t den;
	}fps;
};

class IAudioRender{
protected:
	~IAudioRender(){};

public:
	virtual BOOL InitRender(const RENDERCONFIG &) = 0;
	virtual BOOL DeinitRender() = 0;

	virtual BOOL PushFrame(CSampleBuffer *) = 0;
	virtual const char *GetRenderDescriptor() const = 0;
};

class IAudioRenderFactory
{
protected:
	~IAudioRenderFactory(){};

public:
	virtual BOOL CreateRenderObj(IAudioRender *&) = 0;
	virtual BOOL DestoryRenderObj(IAudioRender *) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	AUDIORENDER_API BOOL GetAudioRenderFactoryObj(IAudioRenderFactory *&);
	AUDIORENDER_API BOOL ReleaseAudioRenderFactoryObj(IAudioRenderFactory *);

#ifdef __cplusplus
}
#endif