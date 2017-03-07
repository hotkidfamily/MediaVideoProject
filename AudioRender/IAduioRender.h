#pragma once

#ifdef AUDIORENDER_EXPORTS
#define AUDIORENDER_API __declspec(dllexport)
#else
#define AUDIORENDER_API __declspec(dllimport)
#endif

struct AudioRenderConfig
{
	AudioRenderConfig(){
		ZeroMemory(this, sizeof(struct AudioRenderConfig));
	}

	HWND hWnd; /* NULL£º full screen, else windowed */
	int32_t channels;
	int32_t sampleRate;
	int32_t bitsPerSample;
	DWORD wavFormat;
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
	virtual BOOL InitRender(const AudioRenderConfig &) = 0;
	virtual BOOL DeinitRender() = 0;

	virtual BOOL PushFrame(CSampleBuffer *) = 0;
	virtual const char *GetRenderDescriptor() const = 0;
};

class IAudioRenderFactory
{
protected:
	~IAudioRenderFactory(){};

public:
	virtual BOOL CreateAudioRenderObj(IAudioRender *&) = 0;
	virtual BOOL DestoryAudioRenderObj(IAudioRender *) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	AUDIORENDER_API BOOL GetAudioRenderFactoryObj(IAudioRenderFactory *&);
	AUDIORENDER_API BOOL ReleaseAudioRenderFactoryObj(IAudioRenderFactory *);

#ifdef __cplusplus
}
#endif