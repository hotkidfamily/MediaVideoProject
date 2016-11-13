#pragma once

#ifdef VIDEORENDER_EXPORTS
#define VIDEORENDER_API __declspec(dllexport)
#else
#define VIDEORENDER_API __declspec(dllimport)
#endif

typedef struct tagRenderFrame{

}RENDER_FRAME, *PRENDER_FRAME;

class IRender{
protected:
	~IRender(){};

public:
	virtual HRESULT Repaint() = 0;
	virtual HRESULT InitRender() = 0;
	virtual HRESULT DeinitRender() = 0;

	virtual HRESULT PushFrame(RENDER_FRAME &frame) = 0;
	virtual HRESULT GetRenderNameStr(const char* &) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*PVIDEO_RENDER_LOG_CALLBACK_FUNC)(int, const char* format, va_list);
	VIDEORENDER_API void SetLogCallback(PVIDEO_RENDER_LOG_CALLBACK_FUNC);

	VIDEORENDER_API IRender *GetRenderObj();
	VIDEORENDER_API void ReleaseRenderObj(IRender *);

#ifdef __cplusplus
}
#endif