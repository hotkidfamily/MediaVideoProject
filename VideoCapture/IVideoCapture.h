#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif

enum VideoCaptureEvent
{
	OPENSUCCESS_EVENT,
	PLUGINOUT_EVENT,
};

class VideoCaptureCallback
{
public:
	virtual void OnFrame(uint8_t *frame, int64_t frameSize, int64_t timestamp) = 0;
	virtual void OnEvent(VideoCaptureEvent) = 0;
};

class IVideoCapture
{
public:
	virtual ~IVideoCapture(){};
	// step 1. register call back function
	virtual void RegisterVideoCaptureCallback(VideoCaptureCallback *) = 0;
	// step 0 or 1, get device list
	virtual HRESULT GetDeviceList(std::vector<char*> &) = 0;
	// step 2, start capture
	virtual HRESULT StartCaptureByIndexWithResolutionAndFramerate(int, int, int, int) = 0;
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