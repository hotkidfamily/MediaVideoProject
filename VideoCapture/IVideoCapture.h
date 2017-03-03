#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <vector>
#include "common\SampleBuffer.h"
#include "common\PixelFormat.h"
typedef void(*PVIDEO_CAPTURE_LOG_CALLBACK_FUNC)(int, const char* format, va_list);

//---------------------------------------------------------
// 
//
//---------------------------------------------------------
typedef std::vector<const TCHAR*> VECT;

struct CAPTURECONFIG
{
	int32_t index;
	int32_t width;
	int32_t height;
	double fps; // number? 
	HWND parentWindow;
	HWND attachWindow;
	DWORD pixelFormat;
	STRING deviceName;
	STRING filePath;

	CAPTURECONFIG()
	{
		index = 0;
		width = height = 0;
		fps = 0.0;
		parentWindow = NULL;
		attachWindow = NULL;
		pixelFormat = PIXEL_FORMAT_NONE;
		filePath.clear();
		deviceName.clear();
	}
};

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
protected:
	~VideoCaptureCallback(){};

public:
	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) = 0;
};

//---------------------------------------------------------
// capture interface 
//---------------------------------------------------------
class IVideoCapture
{
protected:
	virtual ~IVideoCapture(){};

public:	
	// step 0. register call back function, free call back yourself
	virtual void RegisterCallback(VideoCaptureCallback *) = 0;
	// step 1, get device list but using file capture mode
	virtual HRESULT GetDeviceList(VECT &) = 0;
	// step 2, start capture
	virtual HRESULT StartCaptureWithParam(CAPTURECONFIG&) = 0;
	
	// step 3.x other feature support: show property setting window
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) = 0;

	// step 3.x other feature support: show property setting window
	virtual BOOL GetFrame(CSampleBuffer *&pSample) = 0;

	// step 3.x other feature support: show property setting window
	virtual BOOL ReleaseFrame(CSampleBuffer *&pSample) = 0;

	// step 4, close capture
	virtual HRESULT StopCapture() = 0;
	// step 5, release callback
	virtual HRESULT UnRegisterCallback() = 0;
};


#ifdef __cplusplus
extern "C" {
#endif
	VIDEOCAPTURE_API IVideoCapture *GetVideoCaptureObj();
	VIDEOCAPTURE_API void ReleaseVideoCaptureObj(IVideoCapture *);

#ifdef __cplusplus
}
#endif

#endif //__IVIDEOCAPTURE_H__