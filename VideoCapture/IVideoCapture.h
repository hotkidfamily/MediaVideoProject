#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <vector>

//---------------------------------------------------------
// 
//
//---------------------------------------------------------
typedef std::vector<const TCHAR*> VECT;

typedef struct tagDevParam{
	int32_t index;
	int32_t width;
	int32_t height;
	LONGLONG fps; // number? 
	HWND parentWindow;
	HWND attachWindow;

	tagDevParam(){
		ZeroMemory(this, sizeof(struct tagDevParam));
	}
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

typedef struct tagFrameDesc{
	int32_t cbSize;
	int32_t width;
	int32_t height;
	int32_t dataSize;
	uint8_t *dataPtr;
	DWORD pixelFormatInFourCC;
	int64_t ptsStart;
	int64_t ptsEnd;
	int64_t frameStartIdx;
	int64_t frameEndIdx;
	tagFrameDesc(){
		ZeroMemory(this, sizeof(struct tagFrameDesc));
		cbSize = sizeof(struct tagFrameDesc);
	}
}FRAME_DESC;

class VideoCaptureCallback
{
protected:
	~VideoCaptureCallback(){};

public:
	virtual void OnFrame(FRAME_DESC) = 0;
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
	// step 1, get device list
	virtual HRESULT GetDeviceList(VECT &) = 0;
	// step 2, start capture
	virtual HRESULT StartCaptureWithParam(OPEN_DEVICE_PARAM) = 0;
	
	// step 3.x other feature support: show property setting window
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) = 0;

	// step 4, close capture
	virtual HRESULT StopCapture() = 0;
	// step 5, release callback
	virtual HRESULT UnRegisterCallback() = 0;
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