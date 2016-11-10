#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif
//---------------------------------------------------------
// 
//
//---------------------------------------------------------

typedef struct tagDevParam{
	int32_t index;
	int32_t width;
	int32_t height;
	LONGLONG avgFrameIntervalInNs;
}OPEN_DEVICE_PARAM;

//---------------------------------------------------------
// video call back interface
//---------------------------------------------------------
typedef enum tagEventIndex
{
	OPENSUCCESS_EVENT,
	PLUGINOUT_EVENT,
}EVENT_INDEX;

typedef struct tagEventParams{
	void *param1;
	void *param2;
}EVENT_CONTEXT;

class VideoCaptureCallback
{
public:
	virtual void OnFrame(uint8_t *frame, int64_t frameSize, int64_t timestamp) = 0;
	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) = 0;
};

//---------------------------------------------------------
// capture interface 
//---------------------------------------------------------
class IVideoCapture
{
public:
	virtual ~IVideoCapture(){};
	// step 1. register call back function, free call back yourself
	virtual void RegisterCallback(VideoCaptureCallback *) = 0;
	// step 0 or 1, get device list
	virtual HRESULT GetDeviceList(std::vector<const TCHAR*> &) = 0;
	// step 2, start capture
	virtual HRESULT StartCaptureWithParam(OPEN_DEVICE_PARAM) = 0;
	// last step, close capture
	virtual HRESULT StopCapture() = 0;
	// other feature support: show property setting window
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) = 0;
};


#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*PVIDEO_CAPTURE_LOG_CALLBACK_FUNC)(int, const char* format, va_list);
	VIDEOCAPTURE_API void SetLogCallback(PVIDEO_CAPTURE_LOG_CALLBACK_FUNC);

	VIDEOCAPTURE_API IVideoCapture *GetVideoCaptureObj();
	VIDEOCAPTURE_API void ReleaseVideoCaptureObj(IVideoCapture *);

#ifdef __cplusplus
}
#endif

#endif //__IVIDEOCAPTURE_H__